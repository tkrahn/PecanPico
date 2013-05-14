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

#include "ax25.h"
#include "config.h"
#include "modem.h"
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
// Module globals
unsigned short int crc;
int ones_in_a_row;

// Module functions
static void
update_crc(unsigned char bit) 
{
  crc ^= bit;
  if (crc & 1)
    crc = (crc >> 1) ^ 0x8408;  // X-modem CRC poly
  else
    crc = crc >> 1;
}

static void
send_byte(unsigned char byte)
{
  int i;
  for (i = 0; i < 8; i++) {
    update_crc((byte >> i) & 1);
    if ((byte >> i) & 1) {
      // Next bit is a '1'
      if (modem_packet_size >= MODEM_MAX_PACKET * 8)  // Prevent buffer overrun
        return;
      modem_packet[modem_packet_size >> 3] |= (1 << (modem_packet_size & 7));
      modem_packet_size++;
      if (++ones_in_a_row < 5) continue;
    }
    // Next bit is a '0' or a zero padding after 5 ones in a row
    if (modem_packet_size >= MODEM_MAX_PACKET * 8)    // Prevent buffer overrun
      return;
    modem_packet[modem_packet_size >> 3] &= ~(1 << (modem_packet_size & 7));
    modem_packet_size++;
    ones_in_a_row = 0;
  }
}

// Exported functions
void
ax25_send_byte(unsigned char byte)
{
  // Wrap around send_byte, but prints debug info
  send_byte(byte);
#ifdef DEBUG_AX25
  Serial.print((char)byte);
#endif
}

void
ax25_send_sync()
{
  unsigned char byte = 0x00;
  int i;
  for (i = 0; i < 8; i++, modem_packet_size++) {
    if (modem_packet_size >= MODEM_MAX_PACKET * 8)  // Prevent buffer overrun
      return;
    if ((byte >> i) & 1)
      modem_packet[modem_packet_size >> 3] |= (1 << (modem_packet_size & 7));
    else
      modem_packet[modem_packet_size >> 3] &= ~(1 << (modem_packet_size & 7));
  }
}

void
ax25_send_flag()
{
  unsigned char byte = 0x7e;
  int i;
  for (i = 0; i < 8; i++, modem_packet_size++) {
    if (modem_packet_size >= MODEM_MAX_PACKET * 8)  // Prevent buffer overrun
      return;
    if ((byte >> i) & 1)
      modem_packet[modem_packet_size >> 3] |= (1 << (modem_packet_size & 7));
    else
      modem_packet[modem_packet_size >> 3] &= ~(1 << (modem_packet_size & 7));
  }
}

void
ax25_send_string(const char *string)
{
  int i;
  for (i = 0; string[i]; i++) {
    ax25_send_byte(string[i]);
  }
}

void
ax25_send_header(s_address addresses[], int num_addresses)
{
  int i, j;
  modem_packet_size = 0;
  ones_in_a_row = 0;
  crc = 0xffff;
  
  // Send sync ("a bunch of 0s") 
  for (i = 0; i < 60; i++) 
  {
    ax25_send_sync();
  }

  //start the actual frame. Send 3 of them (one empty frame and the real start)
  for (i = 0; i < 3; i++)
  { 
    ax25_send_flag();  
  }
  
  for (i = 0; i < num_addresses; i++) {
    // Transmit callsign
    for (j = 0; addresses[i].callsign[j]; j++)
      send_byte(addresses[i].callsign[j] << 1);
    // Transmit pad
    for ( ; j < 6; j++)
      send_byte(' ' << 1);
    // Transmit SSID. Termination signaled with last bit = 1
    if (i == num_addresses - 1)
      send_byte(('0' + addresses[i].ssid) << 1 | 1);
    else
      send_byte(('0' + addresses[i].ssid) << 1);
  }
  
  // Control field: 3 = APRS-UI frame
  send_byte(0x03);
  
  // Protocol ID: 0xf0 = no layer 3 data
  send_byte(0xf0);

#ifdef DEBUG_AX25
  // Print source callsign
  Serial.println();
  Serial.print(addresses[1].callsign);
  if (addresses[1].ssid) {
    Serial.print('-');
    Serial.print((unsigned int)addresses[1].ssid);
  }
  Serial.print('>');
  // Destination callsign
  Serial.print(addresses[0].callsign);
  if (addresses[0].ssid) {
    Serial.print('-');
    Serial.print((unsigned int)addresses[0].ssid);
  }
  for (i = 2; i < num_addresses; i++) {
    Serial.print(',');
    Serial.print(addresses[i].callsign);
    if (addresses[i].ssid) {
      Serial.print('-');
      Serial.print((unsigned int)addresses[i].ssid);
    }
  }
  Serial.print(':');
#endif
}

void 
ax25_send_footer()
{
  // Save the crc so that it can be treated it atomically
  unsigned short int final_crc = crc;
  
  // Send the CRC
  send_byte(~(final_crc & 0xff));
  final_crc >>= 8;
  send_byte(~(final_crc & 0xff));
  
  // Signal the end of frame
  ax25_send_flag();
  
  // Send sync ("a bunch of 0s") 
//  for (int i = 0; i < 30; i++) 
//  {
//    ax25_send_sync();
//  }

  
#ifdef DEBUG_AX25
  Serial.println();
#endif
}

void
ax25_flush_frame()
{
  // Key the transmitter and send the frame
  modem_flush_frame();
}


