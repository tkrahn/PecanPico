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
 * Michael Smith for his Example of Audio generation with two timers and PWM:
 * http://www.arduino.cc/playground/Code/PCMAudio
 *
 * Ken Shirriff for his Great article on PWM:
 * http://arcfn.com/2009/07/secrets-of-arduino-pwm.html 
 *
 * The large group of people who created the free AVR tools.
 * Documentation on interrupts:
 * http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
 */

#include "config.h"
#include "modem.h"

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

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#if AUDIO_PIN == 3
#  define OCR2 OCR2B
#endif
#if AUDIO_PIN == 11
#  define OCR2 OCR2A
#endif

// Module Constants

// This procudes a "warning: only initialized variables can be placed into
// program memory area", which can be safely ignored:
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734

/*
PROGMEM const prog_uchar sine_table[512] = {
  127, 129, 130, 132, 133, 135, 136, 138, 139, 141, 143, 144, 146, 147, 149, 150, 152, 153, 155, 156, 158, 
  159, 161, 163, 164, 166, 167, 168, 170, 171, 173, 174, 176, 177, 179, 180, 182, 183, 184, 186, 187, 188, 
  190, 191, 193, 194, 195, 197, 198, 199, 200, 202, 203, 204, 205, 207, 208, 209, 210, 211, 213, 214, 215, 
  216, 217, 218, 219, 220, 221, 223, 224, 225, 226, 227, 228, 228, 229, 230, 231, 232, 233, 234, 235, 236, 
  236, 237, 238, 239, 239, 240, 241, 242, 242, 243, 244, 244, 245, 245, 246, 247, 247, 248, 248, 249, 249, 
  249, 250, 250, 251, 251, 251, 252, 252, 252, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 
  254, 254, 255, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 253, 252, 252, 252, 251, 
  251, 251, 250, 250, 249, 249, 249, 248, 248, 247, 247, 246, 245, 245, 244, 244, 243, 242, 242, 241, 240, 
  239, 239, 238, 237, 236, 236, 235, 234, 233, 232, 231, 230, 229, 228, 228, 227, 226, 225, 224, 223, 221, 
  220, 219, 218, 217, 216, 215, 214, 213, 211, 210, 209, 208, 207, 205, 204, 203, 202, 200, 199, 198, 197, 
  195, 194, 193, 191, 190, 188, 187, 186, 184, 183, 182, 180, 179, 177, 176, 174, 173, 171, 170, 168, 167, 
  166, 164, 163, 161, 159, 158, 156, 155, 153, 152, 150, 149, 147, 146, 144, 143, 141, 139, 138, 136, 135, 
  133, 132, 130, 129, 127, 125, 124, 122, 121, 119, 118, 116, 115, 113, 111, 110, 108, 107, 105, 104, 102, 
  101,  99,  98,  96,  95,  93,  91,  90,  88,  87,  86,  84,  83,  81,  80,  78,  77,  75,  74,  72,  71, 
   70,  68,  67,  66,  64,  63,  61,  60,  59,  57,  56,  55,  54,  52,  51,  50,  49,  47,  46,  45,  44, 
   43,  41,  40,  39,  38,  37,  36,  35,  34,  33,  31,  30,  29,  28,  27,  26,  26,  25,  24,  23,  22, 
   21,  20,  19,  18,  18,  17,  16,  15,  15,  14,  13,  12,  12,  11,  10,  10,   9,   9,   8,   7,   7, 
    6,   6,   5,   5,   5,   4,   4,   3,   3,   3,   2,   2,   2,   1,   1,   1,   1,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1, 
    2,   2,   2,   3,   3,   3,   4,   4,   5,   5,   5,   6,   6,   7,   7,   8,   9,   9,  10,  10,  11, 
   12,  12,  13,  14,  15,  15,  16,  17,  18,  18,  19,  20,  21,  22,  23,  24,  25,  26,  26,  27,  28, 
   29,  30,  31,  33,  34,  35,  36,  37,  38,  39,  40,  41,  43,  44,  45,  46,  47,  49,  50,  51,  52, 
   54,  55,  56,  57,  59,  60,  61,  63,  64,  66,  67,  68,  70,  71,  72,  74,  75,  77,  78,  80,  81, 
   83,  84,  86,  87,  88,  90,  91,  93,  95,  96,  98,  99, 101, 102, 104, 105, 107, 108, 110, 111, 113, 
  115, 116, 118, 119, 121, 122, 124, 125
};
*/

// This is a 15% scaled down version of the above
// in order to operate the PWM not too close to its extreme limits. 
// Trying to prevent distortion this way.

PROGMEM const prog_uchar sine_table[512] = {
128,128,128,128,129,129,129,130,130,130,131,131,131,132,132,132,132,133,133,133,134,
134,134,135,135,135,136,136,136,136,137,137,137,138,138,138,138,139,139,139,140,140,
140,140,141,141,141,141,142,142,142,142,143,143,143,143,144,144,144,144,145,145,145,
145,146,146,146,146,146,147,147,147,147,147,148,148,148,148,148,149,149,149,149,149,
149,150,150,150,150,150,150,151,151,151,151,151,151,151,151,151,152,152,152,152,152,
152,152,152,152,152,152,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,152,
152,152,152,152,152,152,152,152,152,152,151,151,151,151,151,151,151,151,151,150,150,
150,150,150,150,149,149,149,149,149,149,148,148,148,148,148,147,147,147,147,147,146,
146,146,146,146,145,145,145,145,144,144,144,144,143,143,143,143,142,142,142,142,141,
141,141,141,140,140,140,140,139,139,139,138,138,138,138,137,137,137,136,136,136,136,
135,135,135,134,134,134,133,133,133,132,132,132,132,131,131,131,130,130,130,129,129,
129,128,128,128,128,127,127,127,126,126,126,125,125,125,124,124,124,123,123,123,123,
122,122,122,121,121,121,120,120,120,119,119,119,119,118,118,118,117,117,117,117,116,
116,116,115,115,115,115,114,114,114,114,113,113,113,113,112,112,112,112,111,111,111,
111,110,110,110,110,109,109,109,109,109,108,108,108,108,108,107,107,107,107,107,106,
106,106,106,106,106,105,105,105,105,105,105,104,104,104,104,104,104,104,104,104,103,
103,103,103,103,103,103,103,103,103,103,102,102,102,102,102,102,102,102,102,102,102,
102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,
102,102,102,103,103,103,103,103,103,103,103,103,103,103,104,104,104,104,104,104,104,
104,104,105,105,105,105,105,105,106,106,106,106,106,106,107,107,107,107,107,108,108,
108,108,108,109,109,109,109,109,110,110,110,110,111,111,111,111,112,112,112,112,113,
113,113,113,114,114,114,114,115,115,115,115,116,116,116,117,117,117,117,118,118,118,
119,119,119,119,120,120,120,121,121,121,122,122,122,123,123,123,123,124,124,124,125,
125,125,126,126,126,127,127,127


};

/* The sine_table is the carrier signal. To achieve phase continuity, each tone
 * starts at the index where the previous one left off. By changing the stride of
 * the index (phase_delta) we get 1200 or 2200 Hz. The PHASE_DELTA_XXXX values
 * can be calculated as:
 * 
 * Fg = frequency of the output tone (1200 or 2200)
 * Fm = sampling rate (PLAYBACK_RATE_HZ)
 * Tt = sine table size (TABLE_SIZE)
 * 
 * PHASE_DELTA_Fg = Tt*(Fg/Fm)
 */

static const unsigned char REST_DUTY       = 127;
static const int TABLE_SIZE                = sizeof(sine_table);
static const unsigned long PLAYBACK_RATE   = F_CPU / 256;    // 62.5KHz @ F_CPU=16MHz; 31.25kHz @ 8MHz
static const int BAUD_RATE                 = 1200;
static const unsigned char SAMPLES_PER_BAUD= (PLAYBACK_RATE / BAUD_RATE); // 52.083333333 / 26.041666667
static const unsigned int PHASE_DELTA_1200 = (((TABLE_SIZE * 1200L) << 7) / PLAYBACK_RATE); // Fixed point 9.7 // 1258 / 2516
static const unsigned int PHASE_DELTA_2200 = (((TABLE_SIZE * 2200L) << 7) / PLAYBACK_RATE); // 2306 / 4613


// Module globals
static unsigned char current_byte;
static unsigned char current_sample_in_baud;    // 1 bit = SAMPLES_PER_BAUD samples
static bool go = false;                         // Modem is on
static unsigned int phase_delta;                // 1200/2200 for standard AX.25
static unsigned int phase;                      // Fixed point 9.7 (2PI = TABLE_SIZE)
static unsigned int packet_pos;                 // Next bit to be sent out
#ifdef DEBUG_MODEM
static int overruns = 0;
static unsigned int slow_isr_time;
static unsigned int slow_packet_pos;
static unsigned char slow_sample_in_baud;
#endif

// The radio (class defined in config.h)
static RADIO_CLASS radio;

// Exported globals
unsigned int modem_packet_size = 0;
unsigned char modem_packet[MODEM_MAX_PACKET];

void modem_setup()
{
  // Configure pins
  pinMode(PTT_PIN, OUTPUT);
  digitalWrite(PTT_PIN, LOW);
  pinMode(AUDIO_PIN, OUTPUT);

  // Start radio
  radio.setup();

  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.
  
  // Source timer2 from clkIO (datasheet p.164)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
  
  // Set fast PWM mode with TOP = 0xff: WGM22:0 = 3  (p.150)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);

#if AUDIO_PIN == 11
  // Do non-inverting PWM on pin OC2A (arduino pin 11) (p.159)
  // OC2B (arduino pin 3) stays in normal port operation:
  // COM2A1=1, COM2A0=0, COM2B1=0, COM2B0=0
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~(_BV(COM2A0) | _BV(COM2B1) | _BV(COM2B0));
#endif  

#if AUDIO_PIN == 3
  // Do non-inverting PWM on pin OC2B (arduino pin 3) (p.159).
  // OC2A (arduino pin 11) stays in normal port operation: 
  // COM2B1=1, COM2B0=0, COM2A1=0, COM2A0=0
  TCCR2A = (TCCR2A | _BV(COM2B1)) & ~(_BV(COM2B0) | _BV(COM2A1) | _BV(COM2A0));
#endif
  
  // No prescaler (p.162)
  TCCR2B = (TCCR2B & ~(_BV(CS22) | _BV(CS21))) | _BV(CS20);

  // Set initial pulse width to the rest position (0v after DC decoupling)
  OCR2 = REST_DUTY;
}

int modem_busy()
{
  return go;
}

int modem_get_powerlevel()
{
  return radio.get_powerlevel();
}

void modem_set_tx_freq(unsigned long freq)
{
  radio.set_freq(freq); 
}

void modem_flush_frame()
{
  phase_delta = PHASE_DELTA_1200;
  phase = 0;
  packet_pos = 0;
  current_sample_in_baud = 0;
  go = true;
  
  // Key the radio
  radio.ptt_on();

  // Clear the overflow flag, so that the interrupt doesn't go off
  // immediately and overrun the next one (p.163).
  TIFR2 |= _BV(TOV2);       // Yeah, writing a 1 clears the flag.

  // Enable interrupt when TCNT2 reaches TOP (0xFF) (p.151, 163)
  TIMSK2 |= _BV(TOIE2);
}

// This is called at PLAYBACK_RATE Hz to load the next sample.
ISR(TIMER2_OVF_vect) {

  if (go) {

    // If done sending packet
    if (packet_pos == modem_packet_size) {
      go = false;             // End of transmission
      OCR2 = REST_DUTY;       // Output 0v (after DC coupling)
      radio.ptt_off();        // Release PTT
      TIMSK2 &= ~_BV(TOIE2);  // Disable playback interrupt.
      goto end_isr;           // Done, gather ISR stats
    }

    // If sent SAMPLES_PER_BAUD already, go to the next bit
    if (current_sample_in_baud == 0) {    // Load up next bit
      if ((packet_pos & 7) == 0)          // Load up next byte
        current_byte = modem_packet[packet_pos >> 3];
      else
        current_byte = current_byte / 2;  // ">>1" forces int conversion
      if ((current_byte & 1) == 0) {
        // Toggle tone (1200 <> 2200)
        phase_delta ^= (PHASE_DELTA_1200 ^ PHASE_DELTA_2200);
      }
    }
    
    phase += phase_delta;
    
    OCR2 = pgm_read_byte_near(sine_table + ((phase >> 7) & (TABLE_SIZE - 1)));
    
    if(++current_sample_in_baud == SAMPLES_PER_BAUD) {
      current_sample_in_baud = 0;
      packet_pos++;
    }
  }
 
end_isr:
#ifdef DEBUG_MODEM
  unsigned int t = (unsigned int) TCNT2;
  // Signal overrun if we received an interrupt while processing this one
  if (TIFR2 & _BV(TOV2)) {
    overruns++;
    t += 256;
  }
  // Keep the slowest execution time in slow_isr_time
  if (t > slow_isr_time) {
    slow_isr_time = t;
    slow_packet_pos = packet_pos;
    slow_sample_in_baud = current_sample_in_baud;
  }
#endif
  return;
}

#ifdef DEBUG_MODEM
void modem_debug()
{
  Serial.print("t=");
  Serial.print(slow_isr_time);
  Serial.print(", pos=");
  Serial.print(slow_packet_pos);
  Serial.print(", sam=");
  Serial.println(slow_sample_in_baud, DEC);
  slow_isr_time = 0;
  if (overruns) {
    Serial.print("MODEM OVERRUNS: ");
    Serial.println(overruns);
    overruns = 0;
  } 
}
#endif
