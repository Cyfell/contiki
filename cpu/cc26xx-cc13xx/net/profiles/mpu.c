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
#include "mpu.h"
#include "board-peripherals.h"

/*---------------------------------------------------------------------------*/
uint8_t actualise_mpu(bt_size_t *database){
  uint16_t value;
  uint8_t iterator;

  iterator = 0;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_X);
  PRINTF("value : 0x%X\n", value);
  database->value.u128.data[iterator++] = value & 0x00ff;
  database->value.u128.data[iterator++] = (value & 0xff00) >>8;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_Y);
  PRINTF("value : 0x%X\n", value);
  database->value.u128.data[iterator++] = value & 0x00ff;
  database->value.u128.data[iterator++] = (value & 0xff00) >>8;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_Z);
  PRINTF("value : 0x%X\n", value);
  database->value.u128.data[iterator++] = value & 0x00ff;
  database->value.u128.data[iterator++] = (value & 0xff00) >>8;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_X);
  PRINTF("value : 0x%X\n", value);
  database->value.u128.data[iterator++] = value & 0x00ff;
  database->value.u128.data[iterator++] = (value & 0xff00) >>8;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_Y);
  PRINTF("value : 0x%X\n", value);
  database->value.u128.data[iterator++] = value & 0x00ff;
  database->value.u128.data[iterator++] = (value & 0xff00) >>8;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_Z);
    PRINTF("value : 0x%X\n", value);
  database->value.u128.data[iterator++] = value & 0x00ff;
  database->value.u128.data[iterator++] = (value & 0xff00) >>8;
  database->type = BT_SIZEMPU;
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t enable_disable_mpu(uint8_t *data){
  switch(data[READ_RESPONSE_DATA_OFFSET]){
    case 1:
    PRINTF("ACTIVATION CAPTEUR\n");
    SENSORS_ACTIVATE(mpu_9250_sensor);
      break;
    case 0:
    PRINTF("DESACTIVATION CAPTEUR");
    SENSORS_DEACTIVATE(mpu_9250_sensor);
      break;
    default:
      return ATT_ECODE_INVALID_PDU;
  }
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t get_status_mpu(bt_size_t *database){
  database->type = BT_SIZE8;
  database->value.u8 = (uint8_t) mpu_9250_sensor.status(SENSORS_ACTIVE);
  PRINTF("status mpu sensor : 0x%X\n", mpu_9250_sensor.status(SENSORS_ACTIVE));
  return SUCCESS;
}
