/* pecanpico2 copyright (C) 2013  KT5TK
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

#ifndef __RADIO_SI446X_H__
#define __RADIO_SI446X_H__

#include "radio.h"

class RadioSi446x : public Radio {
  public:
    virtual void setup();
    virtual void ptt_on();
    virtual void ptt_off();
    virtual void set_freq(unsigned long freq);
    virtual int get_powerlevel();
  private:
    void SendCmdReceiveAnswer(int byteCountTx, int byteCountRx, const char* pData);
    void reset(void);
    void setModem(void);
    void start_tx(void);
    void stop_tx(void);
    void tune_tx(void);
    void setFrequency(unsigned long freq); 
    

 
// PIN Definitions:
  #define RADIO_SDN_PIN   7
  #define VCXO_ENABLE_PIN 8

  #define SCKpin  13   // SCK
  #define SSpin  10    // SS
  #define MOSIpin 11   // MOSI
  #define MISOpin 12   // MISO

 
    
};

#endif
