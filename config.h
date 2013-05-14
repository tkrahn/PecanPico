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

#ifndef __CONFIG_H__
#define __CONFIG_H__


// --------------------------------------------------------------------------
// THIS IS THE TRACKUINO FIRMWARE CONFIGURATION FILE. YOUR CALLSIGN AND
// OTHER SETTINGS GO HERE.
//
// NOTE: all pins are Arduino based, not the Atmega chip. Mapping:
// http://www.arduino.cc/en/Hacking/PinMapping
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
// APRS config (aprs.c)
// --------------------------------------------------------------------------

// Set your callsign and SSID here. Common values for the SSID are
// (from http://zlhams.wikidot.com/aprs-ssidguide):
//
// - Balloons:  11
// - Cars:       9
// - Buoy:       8
// - Home:       0
// - IGate:      5
//
#define S_CALLSIGN      "KT5TK"
#define S_CALLSIGN_ID   2

// Destination callsign: APRS (with SSID=0) is usually okay.
//#define D_CALLSIGN      "APZ"  // APZ = Experimental 
#define D_CALLSIGN      "APECAN"  // APExxx = Telemetry devices (that's what Pecan actually is) 

#define D_CALLSIGN_ID   0


// APRS Symbol
// Consists of Symbol table (/ or \ or a dew overlay symbols) and the symbol ID
#define APRS_SYMBOL_TABLE '/' // Default table
//#define APRS_SYMBOL_TABLE '\\' // Secondary table (needs to be escaped => two \)

//#define APRS_SYMBOL_ID    '>' // /O = Car
#define APRS_SYMBOL_ID    'O' // /O = Balloon
//#define APRS_SYMBOL_ID    'N' // \N = Buoy

// Digipeating paths:
// (read more about digipeating paths here: http://wa8lmf.net/DigiPaths/ )
// The recommended digi path for a balloon is WIDE2-1 or pathless. The default
// is to use WIDE2-1. Comment out the following two lines for pathless:
#define DIGI_PATH1      "WIDE2"
#define DIGI_PATH1_TTL  1
//#define DIGI_PATH2      "WIDE2"
//#define DIGI_PATH2_TTL  1

// If we want to pass selected packets through the International Space Station
#define DIGI_PATH1_SAT "ARISS"
#define DIGI_PATH1_TTL_SAT   0
//#define DIGI_PATH2_SAT "SGATE"
//#define DIGI_PATH2_TTL_SAT   0


// APRS comment: this goes in the comment portion of the APRS message. You
// might want to keep this short. The longer the packet, the more vulnerable
// it is to noise. 
//#define APRS_COMMENT    "http://w5acm.net"
#define APRS_COMMENT    "http://tkrahn.net"
//#define APRS_COMMENT    "Testing Pecan"




// Pressure in Pascal at sea level. 
// Can be adjusted at launch date to get exact altitude from BMP085
#define P0 101325.0  


// --------------------------------------------------------------------------
// AX.25 config (ax25.cpp)
// --------------------------------------------------------------------------

// TX delay in milliseconds
#define TX_DELAY      300   // default was 300

// --------------------------------------------------------------------------
// Tracker config (trackuino.cpp)
// --------------------------------------------------------------------------

// APRS_PERIOD is the period between transmissions. Since we're not listening
// before transmitting, it may be wise to choose a "random" value here JUST
// in case someone else is transmitting at fixed intervals like us. 61000 ms
// is the default (1 minute and 1 second).
//
// Low-power transmissions on occasional events (such as a balloon launch)
// might be okay at lower-than-standard APRS periods (< 10m). Check with/ask
// permision to local digipeaters beforehand.
// #define APRS_PERIOD   25000UL

// APRS_PERIOD is here replaced with APRS_PERIOD_SECONDS because we use the 
// watchdog timer to save more power.
#define APRS_PERIOD_SECONDS 120

// Set any value here (in ms) if you want to delay the first transmission
// after resetting the device.
#define APRS_DELAY    0UL

// GPS baud rate (in bits per second)
#define GPS_BAUDRATE  9600   //4800 for Argentdata High Altitude GPS, 9600 for Venus or uBlox MAX-6Q

// GPS Power Pin
// GPS receivers are real power hogs. To save power we might want to switch the 
// GPS module completely off when it's no longer needed (when we already have
// a valid GPS position for this cycle).
#define GPS_POWER_PIN 5

// Note that the Venus board has a GPIO_24 jumper. By default it is set to GND
// which means that the receiver is running full power in high resolution
// mode. Unless you use the trackuino on a rocket, you should change this 
// jumper with a fine solder iron to the other side (+3.3 V). Then the Venus
// will only draw enough power to supply valid NMEA data to the serial port.
// This is about 70 mA before it gets a lease, but it comes quickly down to
// ~ 40 mA. The board has also a BATT pin. If you permanently power this up 
// with 3.3V then the main power can be switched on and off from the Arduino
// with a transistor through the GPS_POWER_PIN. The GPS fix then only takes a 
// second.

#define GPS_RESET_PIN 9
// The Venus auto reset usually doesn't work. We use this line to force a reset
// pulse from the Arduino to the Venus module each time it should restart.

// --------------------------------------------------------------------------
// Modem config (modem.cpp)
// --------------------------------------------------------------------------

// AUDIO_PIN is the audio-out pin. The audio is generated by timer 2 using
// PWM, so the only two options are pins 3 and 11.
// Pin 11 doubles as MOSI, so I suggest using pin 3 for PWM and leave 11 free
// in case you ever want to interface with an SPI device.
#define AUDIO_PIN       3

// Radio: I've tested trackuino with two different radios:
// Radiometrix HX1 and SRB MX146. The interface with these devices
// is implemented in their respective radio_*.cpp files, and here
// you can choose which one will be hooked up to the tracker.
// The tracker will behave differently depending on the radio used:
//
// RadioHx1 (Radiometrix HX1):
// - Time from PTT-on to transmit: 5ms (per datasheet)
// - PTT is TTL-level driven (on=high) and audio input is 5v pkpk
//   filtered and internally DC-coupled by the HX1, so both PTT
//   and audio can be wired directly. Very few external components
//   are needed for this radio, indeed.
//
// RadioMx146 (SRB MX146):
// - Time from PTT-on to transmit: signaled by MX146 (pin RDY)
// - Uses I2C to set freq (144.8 MHz) on start
// - I2C requires wiring analog pins 4/5 (SDA/SCL) via two level
//   converters (one for each, SDA and SCL). DO NOT WIRE A 5V ARDUINO
//   DIRECTLY TO THE 3.3V MX146, YOU WILL DESTROY IT!!!
//
//   I2C 5-3.3v LEVEL TRANSLATOR:
//
//    +3.3v o--------+-----+      +---------o +5v
//                   /     |      /
//                R  \     |      \ R
//                   /    G|      /
//              3K3  \   _ L _    \ 4K7
//                   |   T T T    |
//   3.3v device o---+--+|_| |+---+---o 5v device
//                     S|     |D
//                      +-|>|-+
//                             N-channel MOSFET
//                           (BS170, for instance)
//
//   (Explanation of the lever translator:
//   http://www.neoteo.com/adaptador-de-niveles-para-bus-i2c-3-3v-5v.neo)
//
// - Audio needs a low-pass filter (R=8k2 C=0.1u) plus DC coupling
//   (Cc=10u). This also lowers audio to 500mV peak-peak required by
//   the MX146.
//
//                   8k2        10uF
//   Arduino out o--/\/\/\---+---||---o
//                     R     |     Cc
//                          ===
//                     0.1uF | C
//                           v
//
// - PTT is pulled internally to 3.3v (off) or shorted (on). Use
//   an open-collector BJT to drive it:
//        
//                             o MX146 PTT
//                             |
//                    4k7    b |c
//   Arduino PTT o--/\/\/\----K  (Any NPN will do)
//                     R       |e
//                             |
//                             v GND
// 
// - Beware of keying the MX146 for too long, you will BURN it.
//
// So, summing up. Options are:
//
// - RadioMx146
// - RadioHx1
// - RadioAdf7012
// - RadioSi446x
#define RADIO_CLASS   RadioSi446x

// --------------------------------------------------------------------------
// Radio config (radio_*.cpp)
// --------------------------------------------------------------------------

// This is the PTT pin to enable the transmitter and the VCXO; HIGH = on                
#define PTT_PIN           8

// The ADL5531 PA is switched on separately with the TPS77650 Enable pin; LOW = on
//#define TX_PA_PIN         6 // Not available at PecanPico

// This is the pin used by the MX146 radio to signal full RF
//#define MX146_READY_PIN   2 // Not available at PecanPico

#define VCXO_FREQ 27000000L

//#define ADF7012_CRYSTAL_FREQ 19440000L
//#define ADF7012_CRYSTAL_FREQ 10000000L

#define ADF7012_CRYSTAL_DIVIDER 15    // [1..15]

#define ADF7012_TX_DATA_PIN 7

// Some radios are frequency agile. Therefore we can set the (default) frequency here:
#define RADIO_FREQUENCY   144390000UL
//#define RADIO_FREQUENCY   432900000UL
//#define RADIO_FREQUENCY   223390000UL

// In other regions the APRS frequencies are different. Our balloon may travel
// from one region to another, so we may QSY according to GPS defined geographical regions
// Here we set some regional frequencies:

#define RADIO_FREQUENCY_REGION1      144800000UL // Europe & Africa
#define RADIO_FREQUENCY_REGION2      144390000UL // North and south America (Brazil is different)
//#define RADIO_FREQUENCY_REGION2      432900000UL
//#define RADIO_FREQUENCY_REGION2      223390000UL

#define RADIO_FREQUENCY_JAPAN        144660000UL
#define RADIO_FREQUENCY_CHINA        144640000UL
#define RADIO_FREQUENCY_BRAZIL       145570000UL
#define RADIO_FREQUENCY_AUSTRALIA    145175000UL
#define RADIO_FREQUENCY_NEWZEALAND   144575000UL 
#define RADIO_FREQUENCY_THAILAND     145525000UL


#define RADIO_FREQUENCY_ARISS        145825000UL // For remote locations we may get lucky with ISS flying by? 
//#define RADIO_FREQUENCY_CW           144050000UL
//#define RADIO_FREQUENCY_TESTING      147435000UL // test area to see if it works


/*
    144.390 MHz - Chile, Indonesia, North America
    144.575 MHz - New Zealand 
    144.660 MHz - Japan
    144.640 MHz - China
    144.800 MHz - South Africa, Europe, Russia
    144.930 MHz - Argentina, Uruguay (no confirmation found. Hope this is not a typo?)
    145.175 MHz - Australia
    145.570 MHz - Brazil
    145.525 MHz - Thailand

*/

// --------------------------------------------------------------------------
// Sensors config (sensors.cpp)
// --------------------------------------------------------------------------

// Most of the sensors.cpp functions use internal reference voltages (either
// AVCC or 1.1V). If you want to use an external reference, you should
// uncomment the following line:
//
// #define USE_AREF
//
// BEWARE! If you hook up an external voltage to the AREF pin and 
// accidentally set the ADC to any of the internal references, YOU WILL
// FRY YOUR AVR.
//
// It is always advised to connect the AREF pin through a pull-up resistor,
// whose value is defined here in ohms (set to 0 if no pull-up):
//
#define AREF_PULLUP           4700
//
// Since there is already a 32K resistor at the ADC pin, the actual
// voltage read will be VREF * 32 / (32 + AREF_PULLUP)
//
// Read more in the Arduino reference docs:
// http://arduino.cc/en/Reference/AnalogReference?from=Reference.AREF

// Pin mappings for the internal / external temperature sensors. VS refers
// to (arduino) digital pins, whereas VOUT refers to (arduino) analog pins.

//#define LM50_VS_PIN     #N/A
#define LM50_VOUT_PIN   2


//#define INTERNAL_LM60_VS_PIN     6
//#define INTERNAL_LM60_VOUT_PIN   0
//#define EXTERNAL_LM60_VS_PIN     7
//#define EXTERNAL_LM60_VOUT_PIN   1

//#define LM335_VS_PIN             8
//#define LM335_VOUT_PIN           2   // adc2



// ADC inputs
#define ADC0_PIN                 A0 // Battery
#define ADC1_PIN                 A1 // Custom
#define ADC2_PIN                 A2 // Custom
#define ADC3_PIN                 A3 // Custom
#define ADC6_PIN                 A6 // ADF7012 MUXOUT
#define ADC7_PIN                 A7 // uBlox Voltage


// Units for temperature sensors (Added by: Kyle Crockett)
// 1 = Celsius, 2 = Kelvin, 3 = Fahrenheit
#define TEMP_UNIT 1

// Calibration value in the units selected. Use integer only.
#define CALIBRATION_VAL 0

// --------------------------------------------------------------------------
// Buzzer config (buzzer.cpp)
// --------------------------------------------------------------------------

// This is the buzzer frequency. Choose one that maximizes the output volume
// according to your buzzer's datasheet. It must be between L and 65535,
// where L = F_CPU / 65535 and F_CPU is the clock frequency in hertzs. For
// Arduinos (16 MHz), that gives a low limit of 245 Hz.
//#define BUZZER_FREQ             895     // Hz

// These are the number of seconds the buzzer will stay on/off alternately
//#define BUZZER_ON_TIME          2       // secs
//#define BUZZER_OFF_TIME         2       // secs

// This option disables the buzzer above BUZZER_ALTITUDE meters. This is a
// float value, so make it really high (eg. 1000000.0 = 1 million meters)
// if you want it to never stop buzzing.
//#define BUZZER_ALTITUDE         3000.0  // meters (divide by 0.3048 for ft.)

// The buzzer is driven by timer 1, so the options here are pin 9 or 10
//#define BUZZER_PIN              9

// --------------------------------------------------------------------------
// Debug
// --------------------------------------------------------------------------

// This is the LED pin (13 on Arduinos). The LED will be on while the AVR is
// running and off while it's sleeping, so its brightness gives an indication
// of the activity.
#define LED_PIN                 13



// Debug info includes printouts from different modules to aid in testing and
// debugging.
// 
// 1. To properly receive debug information, only connect the Arduino RX pin 
//    to the GPS TX pin, and leave the Arduino TX pin disconnected. 
//
// 2. On the serial monitor, set the baudrate to GPS_BAUDRATE (above),
//    usually 9600.
//
// 3. When flashing the firmware, disconnect the GPS from the RX pin or you
//    will get errors.

// #define DEBUG_GPS    // GPS sentence dump and checksum validation
// #define DEBUG_AX25   // AX.25 frame dump
// #define DEBUG_MODEM  // Modem ISR overrun and profiling
// #define DEBUG_RESET  // AVR reset


#endif
