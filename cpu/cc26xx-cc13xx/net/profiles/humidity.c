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
#include "humidity.h"
#include "board-peripherals.h"

/*---------------------------------------------------------------------------*/
uint8_t actualise_humidity(bt_size_t *database){
  uint32_t value;
  uint16_t hum;

  value = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_TEMP);
  if (value != CC26XX_SENSOR_READING_ERROR) {
    PRINTF("HDC: Temp=%d.%02d C\n", ((uint16_t)value)  / 100, ((uint16_t)value)  % 100);
  } else{
    PRINTF("HDC: Temp Read Error\n");
  }
  // let space for humidity value
  value = value << 16;

  hum = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_HUMIDITY);
  if (hum != CC26XX_SENSOR_READING_ERROR) {
    PRINTF("HDC: Humidity=%d.%02d %%RH\n", hum / 100, hum % 100);
    value += hum;
  } else{
    PRINTF("HDC: Humidity Read Error\n");
  }
  database->type = BT_SIZE32;
  database->value.u32 = (uint32_t) value;
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t enable_disable_humidity(const bt_size_t *new_value){
  switch(new_value->value.u8){
    case 1:
      PRINTF("ACTIVATION CAPTEUR\n");
      SENSORS_ACTIVATE(hdc_1000_sensor);
      break;
    case 0:
      PRINTF("DESACTIVATION CAPTEUR");
      SENSORS_DEACTIVATE(hdc_1000_sensor);
      break;
  }
  return SUCCESS;
}

uint8_t get_status_humidity(bt_size_t *database){
  database->type = BT_SIZE8;
  database->value.u8 = (uint8_t) hdc_1000_sensor.status(SENSORS_ACTIVE);
  PRINTF("status humidity barometer : 0x%X\n", hdc_1000_sensor.status(SENSORS_ACTIVE));
  return SUCCESS;
}
