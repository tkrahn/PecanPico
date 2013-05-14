/* pecanpico2 Si446x Driver copyright (C) 2013  KT5TK
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "config.h"
#include <math.h>
#include "radio_si446x.h"



#include <SPI.h>



#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


unsigned int si446x_powerlevel = 0;
unsigned long active_freq = RADIO_FREQUENCY;

//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(ADC2_PIN, ADC1_PIN); // RX, TX


void RadioSi446x::SendCmdReceiveAnswer(int byteCountTx, int byteCountRx, const char* pData)
{
    
    /* There was a bug in A1 hardware that will not handle 1 byte commands. 
       It was supposedly fixed in B0 but the fix didn't make it at the last minute, so here we go again */
    if (byteCountTx == 1)
        byteCountTx++;
    
    digitalWrite(SSpin,LOW);
    char answer;   
    
//    Serial.print("sent: ");
    
//    for (int j = 0; j < byteCountTx; j++) // Loop through the bytes of the pData
//    {
//      byte wordb = pData[j];
//      Serial.print(wordb,HEX);
//      Serial.print(" ");
//    } 
    
//    Serial.println();
    
    for (int j = 0; j < byteCountTx; j++) // Loop through the bytes of the pData
    {
      byte wordb = pData[j];
      SPI.transfer(wordb);  
    } 
    
    digitalWrite(SSpin,HIGH);

    delayMicroseconds(20);

    digitalWrite(SSpin,LOW);   
    
    int reply = 0x00;
    while (reply != 0xFF)
    {       
       reply = SPI.transfer(0x44);
       Serial.print(reply,HEX);
       Serial.print(" ");
       if (reply != 0xFF)
       {
         digitalWrite(SSpin,HIGH);
         delayMicroseconds(20);
         digitalWrite(SSpin,LOW);   
       }
    }
    
    Serial.println();
    
    Serial.print("rx: ");
    
   
    for (int k = 1; k < byteCountRx; k++)
    {
      Serial.print(SPI.transfer(0x44), HEX);
      Serial.print(" ");
    }
       
    digitalWrite(SSpin,HIGH);
    Serial.println();
    delay(500); // Wait half a second to prevent Serial buffer overflow
}


// Actions ===============================================================

// Config reset ----------------------------------------------------------
void RadioSi446x::reset(void) 
{
  digitalWrite(VCXO_ENABLE_PIN,HIGH);
  Serial.println("VCXO is enabled"); 
  delay(600);
  
  

  digitalWrite(RADIO_SDN_PIN, HIGH);  // active high shutdown = reset
  delay(600);
  digitalWrite(RADIO_SDN_PIN, LOW);   // booting
  Serial.println("Radio is powered up"); 

  // Start talking to the Si446X radio chip

  const char PART_INFO_command[] = {0x01}; // Part Info
  SendCmdReceiveAnswer(1, 9, PART_INFO_command);
  Serial.println("Part info was checked");

//divide VCXO_FREQ into its bytes; MSB first  
  unsigned int x3 = VCXO_FREQ / 0x1000000;
  unsigned int x2 = (VCXO_FREQ - x3 * 0x1000000) / 0x10000;
  unsigned int x1 = (VCXO_FREQ - x3 * 0x1000000 - x2 * 0x10000) / 0x100;
  unsigned int x0 = (VCXO_FREQ - x3 * 0x1000000 - x2 * 0x10000 - x1 * 0x100); 

//POWER_UP
  const char init_command[] = {0x02, 0x01, 0x01, x3, x2, x1, x0};// no patch, boot main app. img, FREQ_VCXO, return 1 byte
  SendCmdReceiveAnswer(7, 1 ,init_command); 

  Serial.println("Radio booted"); 

  const char get_int_status_command[] = {0x20, 0x00, 0x00, 0x00}; //  Clear all pending interrupts and get the interrupt status back
  SendCmdReceiveAnswer(4, 9, get_int_status_command);


  Serial.println("Radio ready");
 
  const char gpio_pin_cfg_command[] = {0x13, 0x02, 0x02, 0x02, 0x02, 0x08, 0x11, 0x00}; //  Set all GPIOs LOW; Link NIRQ to CTS; Link SDO to MISO; Max drive strength
  SendCmdReceiveAnswer(8, 8, gpio_pin_cfg_command);

  Serial.println("LEDs should be switched off at this point");
  
  setFrequency(active_freq);
  Serial.println("Frequency set");  
 
  setModem(); 
  Serial.println("CW mode set");  
  
  tune_tx();
  Serial.println("TX tune");  



}

void RadioSi446x::start_tx()
{
  char change_state_command[] = {0x34, 0x07}; //  Change to TX state
  SendCmdReceiveAnswer(2, 1, change_state_command);

}

void RadioSi446x::stop_tx()
{
  char change_state_command[] = {0x34, 0x03}; //  Change to Ready state
  SendCmdReceiveAnswer(2, 1, change_state_command);

}

void RadioSi446x::tune_tx()
{
  char change_state_command[] = {0x34, 0x05}; //  Change to TX tune state
  SendCmdReceiveAnswer(2, 1, change_state_command);

}




// Configuration parameter functions ---------------------------------------

void RadioSi446x::setModem()
{
  // Set to CW mode
  Serial.println("Setting modem into CW mode");  
  char set_modem_mod_type_command[] = {0x11, 0x20, 0x01, 0x00, 0x00};
  SendCmdReceiveAnswer(5, 1, set_modem_mod_type_command);
  
}



void RadioSi446x::setFrequency(unsigned long freq)
{ 
  
  // Set the output divider according to recommended ranges given in Si446x datasheet  
  int outdiv = 4;
  int band = 0;
  if (freq < 705000000UL) { outdiv = 6;  band = 1;};
  if (freq < 525000000UL) { outdiv = 8;  band = 2;};
  if (freq < 353000000UL) { outdiv = 12; band = 3;};
  if (freq < 239000000UL) { outdiv = 16; band = 4;};
  if (freq < 177000000UL) { outdiv = 24; band = 5;};
  
 
  unsigned long f_pfd = 2 * VCXO_FREQ / outdiv;
  
  unsigned int n = ((unsigned int)(freq / f_pfd)) - 1;
  
  float ratio = (float)freq / (float)f_pfd;
  float rest  = ratio - (float)n;
  

  unsigned long m = (unsigned long)(rest * 524288UL); 
 


// set the band parameter
  unsigned int sy_sel = 8; // 
  char set_band_property_command[] = {0x11, 0x20, 0x01, 0x51, (band + sy_sel)}; //   
  // send parameters
  SendCmdReceiveAnswer(5, 1, set_band_property_command);

// Set the pll parameters
  unsigned int m2 = m / 0x10000;
  unsigned int m1 = (m - m2 * 0x10000) / 0x100;
  unsigned int m0 = (m - m2 * 0x10000 - m1 * 0x100); 
  // assemble parameter string
  char set_frequency_property_command[] = {0x11, 0x40, 0x04, 0x00, n, m2, m1, m0};
  // send parameters
  SendCmdReceiveAnswer(8, 1, set_frequency_property_command);
  

}


// Public functions -----------------------------------------------------------

void RadioSi446x::setup()
{
// Not much radio configuration to do here
// because we initialize the transmitter each time right before we transmit a packet

// Just make sure that the VCXO is on and the transmitter chip is switched off at boot up
  pinMode(VCXO_ENABLE_PIN,  OUTPUT);
  digitalWrite(VCXO_ENABLE_PIN,HIGH);
  Serial.println("VCXO is enabled"); 

  pinMode(RADIO_SDN_PIN, OUTPUT);
  delay(300);
  digitalWrite(RADIO_SDN_PIN, HIGH);  // active high = shutdown
  
  // Set up SPI  
  pinMode(SCKpin,  OUTPUT);
  pinMode(SSpin,   OUTPUT);
  pinMode(MOSIpin, OUTPUT);

  digitalWrite(SSpin, HIGH);  // ensure SS stays high for now
  
  // initialize SPI:
  SPI.begin(); 
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8); // 8 MHz / 8 = 1 MHz
  delay(600);
 
  Serial.println("SPI is initialized"); 

}

void RadioSi446x::ptt_on()
{
  
  digitalWrite(VCXO_ENABLE_PIN, HIGH);
  reset();
  // turn on the blue LED (GPIO2) to indicate TX
  char gpio_pin_cfg_command2[] = {0x13, 0x02, 0x02, 0x03, 0x02, 0x08, 0x11, 0x00}; //  Set GPIO2 HIGH; Link NIRQ to CTS; Link SDO to MISO; Max drive strength
  SendCmdReceiveAnswer(8, 1, gpio_pin_cfg_command2);

  start_tx();
  si446x_powerlevel = 1023;
}

void RadioSi446x::ptt_off()
{
  stop_tx();
  si446x_powerlevel = 0;
  // turn off the blue LED (GPIO2)
  char gpio_pin_cfg_command0[] = {0x13, 0x02, 0x02, 0x02, 0x02, 0x08, 0x11, 0x00}; //  Set all GPIOs LOW; Link NIRQ to CTS; Link SDO to MISO; Max drive strength
  SendCmdReceiveAnswer(8, 1, gpio_pin_cfg_command0);

  digitalWrite(RADIO_SDN_PIN, HIGH);  // active high = shutdown
  //digitalWrite(VCXO_ENABLE_PIN, LOW); //keep enabled for now

}

void RadioSi446x::set_freq(unsigned long freq)
{
  active_freq = freq;
}


int RadioSi446x::get_powerlevel()
{
  return si446x_powerlevel;

}

