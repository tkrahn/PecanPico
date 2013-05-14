/* trackuino copyright (C) 2010  EA5HAV Javi
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

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation.
#if ARDUINO < 22
#error "Oops! We need Arduino 22 or later"
#endif

// Trackuino custom libs
#include "aprs.h"
#include "ax25.h"
//#include "buzzer.h"
#include "config.h"
#include "debug.h"
#include "gps.h"
#include "modem.h"
#include "radio.h"

// only include the selected radio
#if RADIO_CLASS == RadioHx1
  #include "radio_hx1.h"
#endif
#if RADIO_CLASS == RadioMx146
  #include "radio_mx146.h"
#endif
#if RADIO_CLASS == RadioAdf7012
  #include "radio_adf7012.h"
#endif
#if RADIO_CLASS == RadioSi446x
  #include "radio_si446x.h"
#endif

#include "sensors.h"
#include <SoftwareSerial.h>


// Arduino/AVR libs
#include <Wire.h>
#include <SPI.h>

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

//unsigned long next_tx_millis;
volatile int wd_counter;
bool newPositionStillUnknown;
//volatile boolean f_wdt=1;
bool gpsIsOn;
//unsigned int powerlevel;


void disable_bod_and_sleep()
{
  /* This will turn off brown-out detection while
   * sleeping. Unfortunately this won't work in IDLE mode.
   * Relevant info about BOD disabling: datasheet p.44
   *
   * Procedure to disable the BOD:
   *
   * 1. BODSE and BODS must be set to 1
   * 2. Turn BODSE to 0
   * 3. BODS will automatically turn 0 after 4 cycles
   *
   * The catch is that we *must* go to sleep between 2
   * and 3, ie. just before BODS turns 0.
   */
  unsigned char mcucr;

  cli();
  mcucr = MCUCR | (_BV(BODS) | _BV(BODSE));
  MCUCR = mcucr;
  MCUCR = mcucr & (~_BV(BODSE));
  sei();
  sleep_mode();    // Go to sleep
}

void power_save()
{
  /* Enter power saving mode. SLEEP_MODE_IDLE is the least saving
   * mode, but it's the only one that will keep the UART running.
   * In addition, we need timer0 to keep track of time, timer 1
   * to drive the buzzer and timer2 to keep pwm output at its rest
   * voltage. TK: changed to conditional SLEEP_MODE_PWR_DOWN
   */
   if (! modem_busy()) {  // Don't sleep if we're txing.
  
    if (newPositionStillUnknown == true)
    {

      // Here we still need the serial port to
      // capture GPS NMEA data
      set_sleep_mode(SLEEP_MODE_IDLE);
      sleep_enable();
      power_adc_disable();
      power_spi_disable();
      power_twi_disable();
      power_timer1_disable();
      digitalWrite(LED_PIN, LOW);
      sleep_mode();    // Go to sleep

    }
    else
    {
      //Serial.end();    
      // Now that we know the position, the serial 
      // port is no longer needed
      // and we can go to full SLEEP_MODE_PWR_DOWN.
      // Only the watchdog interrupt will wake us up.
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
      // Shut down GPS hardware
    
      digitalWrite(GPS_POWER_PIN, LOW);
      delay(500);   // Give time to discharge power capacitor of GPS board   
      gpsIsOn = false;
      sleep_enable();
      power_adc_disable();
      power_spi_disable();
      power_twi_disable();
      power_timer1_disable();

      digitalWrite(LED_PIN, LOW);
      disable_bod_and_sleep();    // Go to sleep
      //sleep_mode();    // Go to sleep
    }
  

    digitalWrite(LED_PIN, HIGH);
  
    sleep_disable();  // Resume after wake up
    power_all_enable();

  }
}

//****************************************************************
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) 
{
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
//  Serial.println(ww);
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);
}

//****************************************************************  
// Watchdog Interrupt Service / is executed when  watchdog timed out
ISR(WDT_vect) 
{
  wd_counter++; // count the seconds for the sleep period
}


void setup()
{
  Serial.begin(GPS_BAUDRATE);
  pinMode(LED_PIN, OUTPUT);
  pinMode(GPS_RESET_PIN, OUTPUT);
  digitalWrite(GPS_RESET_PIN, HIGH);
  pinMode(GPS_POWER_PIN, OUTPUT);
  digitalWrite(GPS_POWER_PIN, LOW); // keep GPS module off for a second
  delay(1000);
  digitalWrite(GPS_POWER_PIN, HIGH);  // Now turn GPS on.
  delay(1000);
  digitalWrite(GPS_RESET_PIN, LOW);  // Reset the GPS to make 100% sure it starts properly
  delay(100);
  digitalWrite(GPS_RESET_PIN, HIGH);
  pinMode(GPS_RESET_PIN, INPUT);
  gpsIsOn = true;
  
  pinMode(ADC1_PIN, INPUT);
  pinMode(ADC2_PIN, INPUT);
  pinMode(ADC3_PIN, INPUT);
  
#ifdef DEBUG_RESET
  Serial.println("RESET");
#endif
  modem_setup();
//  buzzer_setup();
  sensors_setup();
  gps_setup();

  // Schedule the next transmission within APRS_DELAY ms
//  next_tx_millis = millis() + APRS_DELAY; // now done with watchdog counter
  newPositionStillUnknown = true;
  setup_watchdog(9); // set watchdog timeout to (approximately) 8 seconds
  wd_counter = (int)(APRS_PERIOD_SECONDS / 8); // Transmit right away after switching on.
  blink3();
}

void loop()
{
  int c;

  if (wd_counter >= (int)(APRS_PERIOD_SECONDS / 8)) 
//  if (millis() >= next_tx_millis)
  {
    // make sure the serial port is set properly after sleep period
    //Serial.begin(GPS_BAUDRATE);

    // Show modem ISR stats from the previous transmission
#ifdef DEBUG_MODEM
    modem_debug();
#endif
    aprs_send();

//    next_tx_millis = millis() + APRS_PERIOD;
    wd_counter = 0;
    newPositionStillUnknown = true;
    // Now we need the GPS again to find the new position. 
    if(gpsIsOn == false)
    {  
      pinMode(GPS_POWER_PIN, OUTPUT);    // In case this config was forgotten in a brown out 
      //digitalWrite(GPS_POWER_PIN, HIGH); // turn GPS properly off
      //delay(500);    
      digitalWrite(GPS_POWER_PIN, HIGH);  // turn GPS on
      delay(500);
      gps_setup();
//      if (Serial.available())  // check if Venus GPS is really on and delivers NEMA
//      {
        gpsIsOn = true;
//      }
//      else
//      {
//        pinMode(GPS_RESET_PIN, OUTPUT);    // In case this config was forgotten in a brown out
//        digitalWrite(GPS_RESET_PIN, LOW);  // Reset the Venus GPS to make 100% sure it starts properly
//        delay(100);
//        digitalWrite(GPS_RESET_PIN, HIGH);
//        delay(100);
//        pinMode(GPS_RESET_PIN, INPUT);
//      }
      
    }
  }
  
  if (Serial.available() && newPositionStillUnknown) 
//  if (Serial.available()) 
  {
    c = Serial.read();
    if (gps_decode(c)) 
    {
      // We have received and decoded our location
      newPositionStillUnknown = false;
      
      blink3();  // Show the user that we have a valid GPS lease
      if(gps_check_satellite())
      {
        blink3(); // show that ISS is in view
        // upset the watchdog so that we permanently keep transmitting while ISS is in view
        wd_counter = (int)(APRS_PERIOD_SECONDS / 8) + 20;
      }
      
//      if (gps_altitude > BUZZER_ALTITUDE)
//        buzzer_off();   // In space, no one can hear you buzz
//      else
//        buzzer_on();
    }

  } 
  else 
  {
    power_save();
  }
}


void blink3() // for debug
{
  
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
  delay(100);
    
}
