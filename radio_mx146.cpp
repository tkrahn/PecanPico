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
#include "radio_mx146.h"
#include <Wire.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

const int MAX_RES = 16;
char res[MAX_RES];

void RadioMx146::send_cmd(const char *cmd, int cmd_len, char *res, int res_len)
{
  int i = 0;
  int success;
  Wire.beginTransmission(0x48);
  for (i = 0; i < cmd_len; i++) {
#if defined(ARDUINO) && ARDUINO >= 100
    Wire.write(cmd[i]);
#else
    Wire.send(cmd[i]);
#endif
  }
  success = Wire.endTransmission();
  delay(100);
  
  if (res_len > 0) 
  {
    Wire.requestFrom(0x48, res_len);
    while(Wire.available() > 0 && i < res_len)
    {     
#if defined(ARDUINO) && ARDUINO >= 100
      res[i] = Wire.read();
#else
      res[i] = Wire.receive();
#endif
      i++;
    }
  }
}

void RadioMx146::set_freq(unsigned long freq)
{
  char cmd[5];  
  cmd[0] = 'B';
  *((unsigned long *)(cmd+1)) = freq;
  send_cmd(cmd, 5, res, MAX_RES);
}

int RadioMx146::query_temp()
{
  send_cmd("QT", 2, res, 1);
  return res[0];
}

void RadioMx146::setup()
{
  Wire.begin();   // Join the I2C bus as a master
  set_freq(RADIO_FREQUENCY); // Set the default frequency
  //set_freq(MX146_FREQUENCY_TESTING); // Set the testing frequency
}

void RadioMx146::ptt_on()
{
  digitalWrite(PTT_PIN, HIGH);
  // TODO: should wait for the "RDY" signal
  delay(25);
}

void RadioMx146::ptt_off()
{
  digitalWrite(PTT_PIN, LOW);
}
