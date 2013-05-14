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

#include "config.h"
#include "ax25.h"
#include "gps.h"
#include "aprs.h"
#include "sensors.h"
#include "modem.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

//extern void *__bss_end;
//extern void *__brkval;


// Module globals
static unsigned int telemetry_counter = 0;
static unsigned int loss_of_gps_counter = 0;
static unsigned int ariss_counter = 0;

char gps_aprs_lat_old[10];
char gps_aprs_lon_old[10];
char gps_time_old[7];

/*
struct s_address {
  {const char[7] *d_call, const int *d_id}, 
  {const char[7] *s_call, const int *s_id}, 
#ifdef DIGI_PATH1
  {const char[7] *d1_call, const int *d1_id}, 
#endif
#ifdef DIGI_PATH2
  {const char[7] *d2_call, const int *d2_id}
#endif
} ;



//s_address *addresses;

*/




// changed from const  
struct s_address addresses[] =
{ 
    {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign
    {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
    {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
    {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
};


// Module functions
float meters_to_feet(float m)
{
  // 10000 ft = 3048 m
  return m / 0.3048;
}

long addtime( unsigned long original, unsigned long seconds2add)
{
  
  
  unsigned int hours =   original / 10000UL;
  unsigned int minutes = (original % 10000UL) / 100UL;
  unsigned int seconds = (original % 10000UL) % 100UL;
   
  seconds += seconds2add;
  
  minutes += seconds / 60UL;
  seconds = seconds % 60UL;
  
  hours += minutes / 60UL;
  minutes = minutes % 60UL;
  hours = hours % 24UL;
  
  return 10000UL * (unsigned long)hours + 100UL * (unsigned long)minutes + (unsigned long)seconds;
  
}  

// Exported functions
void aprs_send()
{
  char temp[12];                   // Temperature (int/ext)
  unsigned long frequency;
  float altitude = 0;
  int value;
  short bmp085temp;
  long bmp085pressure;
 

  bmp085temp = bmp085GetTemperature(bmp085ReadUT()) / 10; // Must read temperature from BMP081 before you can read pressure!
  bmp085pressure = (float)bmp085GetPressure(bmp085ReadUP()) ;


  
  // Send a telemetry package
  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_string("T#");                // Telemetry Report 
  telemetry_counter++;
  if (telemetry_counter > 999) {
    telemetry_counter = 0;
  }
  snprintf(temp, 4, "%03d", telemetry_counter);
  ax25_send_string(temp);              // sequential id
  ax25_send_byte(','); 
  
  analogReference(DEFAULT);   
         
// adc0 = Battery Voltage ; Write to T# pos 1
    analogRead(A0);                      // Disregard the 1st conversion after changing ref
    value = analogRead(A0);          // read ADC
    snprintf(temp, 4, "%03d", value/4L);  // Arduino has 1023 max.; APRS has 255 max.; So divide by 4
    ax25_send_string(temp);               // write 8 bit ADC value
    ax25_send_byte(',');  
  
// adc7 = uBlox Voltage ; Write to T# pos 2
    analogRead(A7);                      // Disregard the 1st conversion after changing ref
    value = analogRead(A7);          // read ADC
    snprintf(temp, 4, "%03d", value/4L);  // Arduino has 1023 max.; APRS has 255 max.; So divide by 4
    ax25_send_string(temp);               // write 8 bit ADC value
    ax25_send_byte(',');  


  // fill the 3rd telemetry value with a number that's proportional to the altitude:
  snprintf(temp, 4, "%03d", (long)(meters_to_feet(gps_altitude) + 0.5)/1000L); // Altitude in kfeet
  ax25_send_string(temp);               // write 8 bit value
  ax25_send_byte(',');  

  
// adc3 = PLL MUX, (skip)


// adc6 = ADF7012 MUXOUT; Write to T# pos 4
  snprintf(temp, 4, "%03d", modem_get_powerlevel());
  ax25_send_string(temp);               // write power level from last tx cycle
  ax25_send_byte(',');  
  


// adc3 = Attach a LM50 temp sensor here ; Write to T# pos 5
    analogRead(A3);                      // Disregard the 1st conversion after changing ref
    value = analogRead(A3);          // read ADC
    snprintf(temp, 4, "%03d", value/4L);  // Arduino has 1023 max.; APRS has 255 max.; So divide by 4
    ax25_send_string(temp);               // write 8 bit ADC value
    ax25_send_byte(',');  
  
  
  
/*  
  
  // fill the fourth telemetry value with the loss_of_gps_counter
  snprintf(temp, 4, "%03d", loss_of_gps_counter); 
  ax25_send_string(temp);               // write 8 bit value
  ax25_send_byte(',');   
  
  // fill the fifth telemetry value with the number of ARISS packets transmitted
  snprintf(temp, 4, "%03d", ariss_counter); 
  ax25_send_string(temp);               // write 8 bit value
  ax25_send_byte(',');   
  
*/  


  ax25_send_string("0000000"); // More room to send binary info. Still unused bits...
  //Todo: PLL MUX info
  if(satInView)
  {
    ax25_send_byte('1');

    frequency = RADIO_FREQUENCY_ARISS;
    ariss_counter++;
    if(ariss_counter > 199) // prevent overflow but still produce interpretable graphs
    {
      ariss_counter = 100; // this will likely never happen :) However just in case...
    }
    
      s_address addresses[] = { 
      {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign still APRS
      {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
      {DIGI_PATH1_SAT, DIGI_PATH1_TTL_SAT}, // Digi1 = ARISS
#endif
#ifdef DIGI_PATH2    
      {DIGI_PATH2_SAT, DIGI_PATH2_TTL_SAT}, // Digi2 = SGATE
#endif
    };

  }
  else
  {
    ax25_send_byte('0');
    
    frequency = gps_get_region_frequency();
    
    s_address addresses[] = { 
      {D_CALLSIGN, D_CALLSIGN_ID},  // Destination callsign changed back to normal
      {S_CALLSIGN, S_CALLSIGN_ID},  // Source callsign (-11 = balloon, -9 = car)
#ifdef DIGI_PATH1
      {DIGI_PATH1, DIGI_PATH1_TTL}, // Digi1 (first digi in the chain)
#endif
#ifdef DIGI_PATH2
      {DIGI_PATH2, DIGI_PATH2_TTL}, // Digi2 (second digi in the chain)
#endif
    };   

  }
  
  
  // Announce the next transmission
//  ax25_send_byte(' ');
//  snprintf(temp, 4, "%lu", frequency/1000000UL);  
//  ax25_send_string(temp); 
//  ax25_send_byte('.');
//  snprintf(temp, 7, "%03lu", (frequency%1000000UL)/1000UL);  
//  ax25_send_string(temp); 
//  ax25_send_string("MHz");     // Comment
//  ax25_send_string(" next ISS:"); 
//  snprintf(temp, 7, "%d", iss_lat);
//  ax25_send_string(temp);       // ISS lat
//  ax25_send_byte('/');  
//  snprintf(temp, 7, "%d", iss_lon);
//  ax25_send_string(temp);       // ISS lon
//  ax25_send_byte('/'); 
//  snprintf(temp, 7, "%u", iss_datapoint);   
//  ax25_send_string(temp);
  
//  if(newPositionStillUnknown) // append the last known position in case we lost GPS
//  {
//    ax25_send_string(" old:");     
//    ax25_send_string(old_lease);
//  }
  
// some output for debugging via packet:
// ax25_send_string(gps_date);         // 241211 = Christmas 2011

//  ax25_send_string(dtostrf(gps_get_variable(),7,4,temp)); 
//  ax25_send_byte('/');  
//  ax25_send_string(dtostrf(gps_get_lat(),7,4,temp)); 
//  ax25_send_byte('/');
//  ax25_send_string(dtostrf(gps_get_lon(),7,4,temp)); 
//  ax25_send_byte('/');
//  snprintf(temp, 7, "%lu", gps_get_time());   
//  ax25_send_string(temp);
//  ax25_send_byte('/');  
//  snprintf(temp, 7, "%lu", gps_get_date());   
//  ax25_send_string(temp);
//  snprintf(temp, 7, "%du", p13.Sx);   
//  ax25_send_string(temp);
 
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go



    // Wait a few seconds (Else aprs.fi reports "[Rate limited (< 5 sec)]")
    delay(6000);
    
    
  ax25_send_header(addresses, sizeof(addresses)/sizeof(s_address));
  ax25_send_byte('/');                // Report w/ timestamp, no APRS messaging. $ = NMEA raw data
  
  altitude = gps_get_altitude();
  
  if(newPositionStillUnknown) 
  {
    //use the old position
    strcpy(gps_aprs_lat, gps_aprs_lat_old);
    strcpy(gps_aprs_lon, gps_aprs_lon_old);
    
    snprintf(gps_time, 7, "%06ld", addtime( atol(gps_time_old), APRS_PERIOD_SECONDS ) );
    
    
    //calculate altitude from pressure
    altitude = (float)44330 * (1 - pow((float)bmp085pressure / P0, 0.190295));
//     altitude = 0.0;

  } 
//  else
//  {
//    altitude = gps_get_altitude();
//  } 
  if (altitude < 1000.0) // At low altitudes pressure altitude is more precise than GPS
  {
    altitude = (float)44330 * (1 - pow((float)bmp085pressure / P0, 0.190295));
  }
  // ax25_send_string("021709z");     // 021709z = 2nd day of the month, 17:09 zulu (UTC/GMT)
  ax25_send_string(gps_time);         // 170915 = 17h:09m:15s zulu (not allowed in Status Reports)
  ax25_send_byte('h');
  ax25_send_string(gps_aprs_lat);     // Lat: 38deg and 22.20 min (.20 are NOT seconds, but 1/100th of minutes)
  ax25_send_byte(APRS_SYMBOL_TABLE);                // Symbol table
  ax25_send_string(gps_aprs_lon);     // Lon: 000deg and 25.80 min
  ax25_send_byte(APRS_SYMBOL_ID);                // Symbol: /O=balloon, /-=QTH, \N=buoy
  snprintf(temp, 4, "%03d", (int)(gps_course + 0.5)); 
  ax25_send_string(temp);             // Course (degrees)
  ax25_send_byte('/');                // and
  snprintf(temp, 4, "%03d", (int)(gps_speed + 0.5));
  ax25_send_string(temp);             // speed (knots)
  ax25_send_string("/A=");            // Altitude (feet). Goes anywhere in the comment area
  snprintf(temp, 7, "%06ld", (long)(meters_to_feet(altitude) + 0.5));
  ax25_send_string(temp);
  ax25_send_string(" ");
    
  ax25_send_string(dtostrf((getUBatt()/1000.0),4,1,temp));
  ax25_send_string("V ");
//    ax25_send_string(itoa(sensors_lm335(), temp, 10));
//    ax25_send_string(itoa(sensors_lm50(), temp, 10));

  ax25_send_string(itoa(bmp085temp, temp, 10));
  ax25_send_string("C ");
  
  ax25_send_string(ltoa(bmp085pressure, temp, 10));
  ax25_send_string("Pa ");
  
  
  ax25_send_string(APRS_COMMENT);     // Comment
  
  if(newPositionStillUnknown)
  {
    
    loss_of_gps_counter++;
    if(loss_of_gps_counter >= 1000) // make sure we don't get above 1000
    {
      loss_of_gps_counter = 100; // will show a sawtooth pattern 100 -> 1000 at permanent gps loss.
    }
    ax25_send_string(" GPS loss ");
    snprintf(temp, 4, "%3d", loss_of_gps_counter); 
    ax25_send_string(temp);               // write 8 bit value

  }
  else
  {
    loss_of_gps_counter = 0;
  }
  
  ax25_send_footer();
  ax25_flush_frame();                 // Tell the modem to go
  strcpy(gps_aprs_lat_old, gps_aprs_lat);
  strcpy(gps_aprs_lon_old, gps_aprs_lon);
  strcpy(gps_time_old, gps_time);




  modem_set_tx_freq(frequency); // actually do the QSY
  
}


