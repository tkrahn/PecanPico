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

/* Credit to:
 *
 * cathedrow for this idea on using the ADC as a volt meter:
 * http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */

#include "config.h"
#include "sensors.h"
#include <Wire.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif



#define BMP085_ADDRESS 0x77  // I2C address of BMP085

const unsigned char OSS = 0;  // Oversampling Setting
unsigned char timeout_error = 0;

// Calibration values
//int ac1;
#define BMP085_AC1 (int)bmp085ReadInt(0xAA)

//int ac2; 
#define BMP085_AC2 (int)bmp085ReadInt(0xAC)

//int ac3; 
#define BMP085_AC3 (int)bmp085ReadInt(0xAE)

//unsigned int ac4;
#define BMP085_AC4 (unsigned int)bmp085ReadInt(0xB0)

//unsigned int ac5;
#define BMP085_AC5 (unsigned int)bmp085ReadInt(0xB2)

//unsigned int ac6;
#define BMP085_AC6 (unsigned int)bmp085ReadInt(0xB4)

//int b1;
#define BMP085_B1 (int)bmp085ReadInt(0xB6)
 
int b2;
#define BMP085_B2 (int)bmp085ReadInt(0xB8)

int mb;
#define BMP085_MB (int)bmp085ReadInt(0xBA)

int mc;
#define BMP085_MC (int)bmp085ReadInt(0xBC)

int md;
#define BMP085_MD (int)bmp085ReadInt(0xBE)





// b5 is calculated in bmp085GetTemperature(...), this variable is also used in bmp085GetPressure(...)
// so ...Temperature(...) must be called before ...Pressure(...).
long b5; 




/*
 * sensors_aref: measure an external voltage hooked up to the AREF pin,
 * optionally (and recommendably) through a pull-up resistor. This is
 * incompatible with all other functions that use internal references
 * (see config.h)
 */
#ifdef USE_AREF
void sensors_setup()
{
  // Nothing to set-up when AREF is in use
}

unsigned long sensors_aref()
{
  unsigned long result;
  // Read 1.1V reference against AREF (p. 262)
  ADMUX = _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = (ADCH << 8) | ADCL;

  // millivolts = 1.1 * 1024 * 1000 / result
  result = 1126400 / result;

  // aref = read aref * (32K + AREF_PULLUP) / 32K
  result = result * (32000UL + AREF_PULLUP) / 32000;

  return result;
}
#endif

#ifndef USE_AREF
void sensors_setup()
{
//  pinMode(INTERNAL_LM60_VS_PIN, OUTPUT);
//  pinMode(EXTERNAL_LM60_VS_PIN, OUTPUT);
//  pinMode(LM335_VS_PIN, OUTPUT);
//  pinMode(LM50_VS_PIN,  OUTPUT);

  Wire.begin();
//  bmp085Calibration(); // now done with defines to save RAM

}


int sensors_lm50()
{
  analogReference(DEFAULT);            // Ref=3.3V. 
  analogRead(LM50_VOUT_PIN);            // Disregard the 1st conversion after changing ref (p.256)
  int adc = analogRead(LM50_VOUT_PIN);  // Real read
  int mV = 3300L * adc / 1024L;         // Millivolts
  
  switch (TEMP_UNIT)//Added by: Kyle Crockett
  {
	case 1://C
		return ((mV - 500) / 10) + CALIBRATION_VAL ;    
	break;
	case 2://K
		return ((mV - 500) / 10) + 273 + CALIBRATION_VAL; //C + 273 = K
	break;
	case 3://F
		return (9L * (mV - 500) / 50) + 32+ CALIBRATION_VAL; // (9/5)C + 32 = F
	break;
  };
}

/*
int sensors_lm60(int powerPin, int readPin)
{
  digitalWrite(powerPin, HIGH);   // Turn the LM60 on
//  analogReference(INTERNAL);      // Ref=1.1V. Okay up to 108 degC (424 + 6.25*108 = 1100mV)
  analogReference(DEFAULT);            // Ref=3.3V. 
  analogRead(readPin);            // Disregard the 1st conversion after changing ref (p.256)
  int adc = analogRead(readPin);  // Real read
  digitalWrite(powerPin, LOW);    // Turn the LM60 off 
//  int mV = 1100L * adc / 1024L;   // Millivolts
  int mV = 3300L * adc / 1024L;         // Millivolts
  
  switch (TEMP_UNIT)//Added by: Kyle Crockett
  {
	case 1://C
		return (4L * (mV - 424) / 25)+ CALIBRATION_VAL ;    // Vo(mV) = (6.25*T) + 424 -> T = (Vo - 424) * 100 / 625
	break;
	case 2://K
		return (4L * (mV - 424) / 25) + 273 + CALIBRATION_VAL; //C + 273 = K
	break;
	case 3://F
		return (36L * (mV - 424) / 125) + 32+ CALIBRATION_VAL; // (9/5)C + 32 = F
	break;
  };
}

int sensors_ext_lm60()
{
  return sensors_lm60(EXTERNAL_LM60_VS_PIN, EXTERNAL_LM60_VOUT_PIN);
}

int sensors_int_lm60()
{
  return sensors_lm60(INTERNAL_LM60_VS_PIN, INTERNAL_LM60_VOUT_PIN);
}

int sensors_lm335()
{
 digitalWrite(LM335_VS_PIN, HIGH); // Turn LM335 on
 analogReference(DEFAULT);
 analogRead(LM335_VOUT_PIN);
 int adc = analogRead(LM335_VOUT_PIN);
 digitalWrite(LM335_VS_PIN, LOW); // Turn LM335 off 
 int kelvin = adc * 3300L / 10240L;
 int celsius = kelvin - 273;
 //int farenheit = celsius * 9L / 5L + 32L;
 return celsius;
}  
*/


int getUBatt()
{
  analogReference(DEFAULT);            
  analogRead(0);            // Disregard the 1st conversion after changing ref
  int adc = analogRead(0);  // Real read
  // mV = ( Uref * adc / 1024bit resolution ) / Voltage divider ratio
  // mV = ( Uref * adc / 1024bit resolution ) / (33 kOhm / (33 kOhm + 100 kOhm)) for PecanPico

  // 
  // Uref = 3.3 V
  long mV = adc * 13300L / 1024L;
  
  return (int)mV;
}

/*
int sensors_humidity()
{
  // TO DO
  return 0;
}

int sensors_pressure()
{
  // TO DO
  return 0;
}

int sensors_uv_ray()
{
  // Nice to have at 40 km altitude
  return 0;
}

int sensors_gamma_ray()
{
  // http://www.cooking-hacks.com/index.php/documentation/tutorials/geiger-counter-arduino-radiation-sensor-board
  return 0;
}

int sensors_graviton()
{
  // Wait, what?
  return 0;
}
*/

// Read 1 byte from the BMP085 at 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;
  unsigned int timeout = 2000;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
  {
    timeout--;
    if (timeout == 0)
    {
      timeout_error = 1;
      return '0';
    }  
  }
  
    
  return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;
  unsigned int timeout = 2000;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
  {
    timeout--;
    if (timeout == 0)
    {
      timeout_error = 1;
      return '0';
    }  
  }
  

  msb = Wire.read();
  lsb = Wire.read();
  
  return (int) msb<<8 | lsb;
}

// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
//void bmp085Calibration()
//{
//  ac1 = bmp085ReadInt(0xAA);
//  ac2 = bmp085ReadInt(0xAC);
//  ac3 = bmp085ReadInt(0xAE);
//  ac4 = bmp085ReadInt(0xB0);
//  ac5 = bmp085ReadInt(0xB2);
//  ac6 = bmp085ReadInt(0xB4);
//  b1 = bmp085ReadInt(0xB6);
//  b2 = bmp085ReadInt(0xB8);
//  mb = bmp085ReadInt(0xBA);
//  mc = bmp085ReadInt(0xBC);
//  md = bmp085ReadInt(0xBE);
//}

	// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  unsigned int timeout = 2000;
  
  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
  {
    timeout--;
    if (timeout == 0)
    {
      timeout_error = 1;
      return 0;
    }  
  }

    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}


// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
short bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;
  
  x1 = (((long)ut - (long)BMP085_AC6)*(long)BMP085_AC5) >> 15;
  x2 = ((long)BMP085_MC << 11)/(x1 + BMP085_MD);
  b5 = x1 + x2;
  if (timeout_error == 1)
  {
    timeout_error = 0;
    return 0;
  }
  else
  {
    return ((b5 + 8)>>4);
  }  
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (BMP085_B2 * (b6 * b6)>>12)>>11;
  x2 = (BMP085_AC2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)BMP085_AC1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (BMP085_AC3 * b6)>>13;
  x2 = (BMP085_B1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (BMP085_AC4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  if (timeout_error == 1)
  {
    timeout_error = 0;
    return 0;
  }
  else
  {  
    return p;
  }
}


#endif
