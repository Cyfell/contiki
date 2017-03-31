/*
 * Copyright (c) 2017, Arthur Courtel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Arthur Courtel <arthurcourtel@gmail.com>
 *
 */
/*---------------------------------------------------------------------------*/

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#include "../ble-att.h"
#include "temp.h"
#include "board-peripherals.h"

/*---------------------------------------------------------------------------*/
uint8_t actualise_temp(bt_size_t *value){
  int temp, tobj;
  temp = tmp_007_sensor.value(TMP_007_SENSOR_TYPE_ALL);

  if(temp == CC26XX_SENSOR_READING_ERROR)
    return 0; //ERROR

  temp = tmp_007_sensor.value(TMP_007_SENSOR_TYPE_AMBIENT);
  PRINTF("TEMP Ambiant: %02X\n", temp);

  temp = temp << 16;

  tobj = tmp_007_sensor.value(TMP_007_SENSOR_TYPE_OBJECT);
  PRINTF("TEMP Ambiant: %02X\n", temp);
  temp += tobj;
  value->type = BT_SIZE32;
  value->value.u32 = temp;
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t enable_disable_temp(uint8_t * data){
  switch(data[3]){
    case 1:
    PRINTF("ACTIVATION CAPTEUR\n");
    SENSORS_ACTIVATE(tmp_007_sensor);
      break;
    case 0:
    PRINTF("DESACTIVATION CAPTEUR");
    SENSORS_DEACTIVATE(tmp_007_sensor);
      break;
    default:
      return 0; //ERROR
  }
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t get_status_temp(bt_size_t *database){
  database->type = BT_SIZE8;
  database->value.u8 = (uint8_t) tmp_007_sensor.status(SENSORS_ACTIVE);
  PRINTF("status temp sensor : 0x%X\n", tmp_007_sensor.status(SENSORS_ACTIVE));
  return SUCCESS;
}
