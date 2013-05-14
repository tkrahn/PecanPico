/* pecan copyright (C) 2012  KT5TK
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

#ifndef __RADIO_ADF7012_H__
#define __RADIO_ADF7012_H__

#include "radio.h"

class RadioAdf7012 : public Radio {
  public:
    virtual void setup();
    virtual void ptt_on();
    virtual void ptt_off();
    virtual void set_freq(unsigned long freq);
    virtual int get_powerlevel();
  private:
    void send_cmd(const char *cmd, int cmd_len, char *res_adf7012, int res_len);
    void adf_reset_config(void);
    void adf_reset_register_zero(void);
    void adf_reset_register_one(void);
    void adf_reset_register_two(void);
    void adf_reset_register_three(void);
    void adf_write_config(void);
    void adf_write_register_zero(void);
    void adf_write_register_one(void);
    void adf_write_register_two(void);
    void adf_write_register_three(void);
    void adf_write_register(unsigned long data);
    void adf_reset(void);
    int adf_lock(void);
    int adf_locked(void);
    void lock_test(void);
    void divider_test(void);
    void change(void);  
    
// Register Constants ========================================================

// Register 1 ----------------------------------------------------------------
#define ADF_OUTPUT_DIVIDER_BY_1 0
#define ADF_OUTPUT_DIVIDER_BY_2 1
#define ADF_OUTPUT_DIVIDER_BY_4 2
#define ADF_OUTPUT_DIVIDER_BY_8 3



// Register 1 ----------------------------------------------------------------
#define ADF_PRESCALER_4_5 0
#define ADF_PRESCALER_8_9 1
#define ADF_PRESCALER ADF_PRESCALER_8_9 

// Register 2 ----------------------------------------------------------------
#define ADF_MODULATION_FSK 0
#define ADF_MODULATION_GFSK 1
#define ADF_MODULATION_ASK 2
#define ADF_MODULATION_OOK 3

// Register 3 ----------------------------------------------------------------
#define ADF_CP_CURRENT_0_3 0
#define ADF_CP_CURRENT_0_9 1
#define ADF_CP_CURRENT_1_5 2
#define ADF_CP_CURRENT_2_1 3
#define ADF_MUXOUT_LOGIC_LOW 0
#define ADF_MUXOUT_LOGIC_HIGH 1
#define ADF_MUXOUT_REG_READY 3
#define ADF_MUXOUT_DIGITAL_LOCK 4
#define ADF_MUXOUT_ANALOGUE_LOCK 5
#define ADF_MUXOUT_R_DIVIDER_2 6
#define ADF_MUXOUT_N_DIVIDER_2 7
#define ADF_MUXOUT_RF_R_DIVIDER 8
#define ADF_MUXOUT_DATA_RATE 9
#define ADF_MUXOUT_BATT_2_35 10
#define ADF_MUXOUT_BATT_2_75 11
#define ADF_MUXOUT_BATT_3 12
#define ADF_MUXOUT_BATT_3_25 13
#define ADF_MUXOUT_TEST_MODE 14
#define ADF_MUXOUT_SD_TEST_MODE 15
#define ADF_LD_PRECISION_3_CYCLES 0
#define ADF_LD_PRECISION_5_CYCLES 1
 
// PIN Definitions:
  #define SCKpin  13   // SCK
  #define SSpin  10    // SS
  #define MOSIpin 11   // MOSI

#define ADF7012_CRYSTAL_FREQ VCXO_FREQ
    
};

#endif
