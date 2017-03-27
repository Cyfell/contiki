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
#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#include "net/ble-att.h"
#include "net/profiles/gatt-sensors.h"
#include "uuid.h"
#include "board.h"
#include <stdlib.h>
#include <string.h>
#include "lib/memb.h"

#define UUID_PRIMARY_DECLARATION          {	0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_CHARACTERISTIC_DECLARATION   {	0x00, 0x00, 0x28, 0x03, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_DEVICE_NAME                  {	0x00, 0x00, 0x2A, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_CONTIKI_VERSION              {	0x00, 0x00, 0x2A, 0x11, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

#define UUID_TEMP_SERVICE                 {	0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_TEMP_DATA                    {	0x00, 0x00, 0xAA, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_TEMP_ED                      {	0x00, 0x00, 0xAA, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

#define UUID_HUMIDITY_SERVICE             {	0x00, 0x00, 0xAA, 0x10, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_HUMIDITY_DATA                {	0x00, 0x00, 0xAA, 0x11, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_HUMIDITY_ED                  {	0x00, 0x00, 0xAA, 0x12, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

#define UUID_BAROMETER_SERVICE            {	0x00, 0x00, 0xAA, 0x20, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_BAROMETER_DATA               {	0x00, 0x00, 0xAA, 0x21, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_BAROMETER_ED                 {	0x00, 0x00, 0xAA, 0x22, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

#define UUID_LUXOMETER_SERVICE            {	0x00, 0x00, 0xAA, 0x30, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_LUXOMETER_DATA               {	0x00, 0x00, 0xAA, 0x31, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_LUXOMETER_ED                 {	0x00, 0x00, 0xAA, 0x32, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

#define UUID_MPU_SERVICE                  {	0x00, 0x00, 0xAA, 0x40, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_MPU_DATA                     {	0x00, 0x00, 0xAA, 0x41, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_MPU_ED                       {	0x00, 0x00, 0xAA, 0x42, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }

#define UUID_LED_SERVICE                  {	0x00, 0x00, 0xAA, 0x50, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_LED_DATA                     {	0x00, 0x00, 0xAA, 0x51, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
#define UUID_LED_ED                       {	0x00, 0x00, 0xAA, 0x52, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }



#define WRITE_REQUEST_HEADER 3
// only accept 1 byte in write request
#define WRITE_REQUEST_LEN_MAX 4

// static uint8_t no_action(bt_size_t){
//
//   return SUCCESS;
// }

static const attribute_t *list_attr[]=
{
  &(attribute_t){ // PRIMARY SERVICE DECLARATION : GENERIC ACCESS SERVICE
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_SIZE16,
    .att_value.value.u16 = 0x1800,
    .att_uuid.data = UUID_PRIMARY_DECLARATION,
    .properties.write = 1,
    .properties.read = 1,
    .att_handle =0x0001,
  },
  &(attribute_t){ // CHAR DECLARATION : DEVICE NAME
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_CHARACTERISTIC,
    .att_value.value.u64 = 0x020300012A,
    .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0002,
  },
  &(attribute_t){ // DEVICE NAME
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_SIZE_STR,
    .att_value.value.str = BOARD_STRING,
    .att_uuid.data = UUID_DEVICE_NAME,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0003,
  },
  &(attribute_t){ // PRIMARY SERVICE DECLARATION : INFORMATION SERVICE
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_SIZE16,
    .att_value.value.u16 = 0x180A,
    .att_uuid.data = UUID_PRIMARY_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0004,
  },
  &(attribute_t){ // CHAR DECLARATION : CONTIKI VERSION
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_CHARACTERISTIC,
    .att_value.value.u64 = 0x020300112A,
    .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0005,
  },
  &(attribute_t){ // CONTIKI VERSION
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_SIZE_STR,
    .att_value.value.str = CONTIKI_VERSION_STRING,
    .att_uuid.data = UUID_CONTIKI_VERSION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0006,
  },
  &(attribute_t){ // PRIMARY SERVICE DECLARATION : TEMP IR
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_SIZE128,
    .att_value.value.u128.data = UUID_TEMP_SERVICE,
    .att_uuid.data = UUID_PRIMARY_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0007,
  },
  &(attribute_t){ // CHAR DECLARATION : TEMP DATA
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_CHARACTERISTIC,
    .att_value.value.u64 = 0x02030001AA,
    .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0008,
  },
  &(attribute_t){ // TEMP DATA
    .get_action = actualise_temp,
    .set_action = NULL,
    .att_value.type = BT_SIZE16,
    .att_value.value.u16 = 0x0,
    .att_uuid.data = UUID_TEMP_DATA,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x0009,
  },
  &(attribute_t){ // CHAR DECLARATION : TEMP ED
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_CHARACTERISTIC,
    .att_value.value.u64 = 0x020B0002AA,
    .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x000A,
  },
  &(attribute_t){ // TEMP ED
    .get_action = NULL,
    .set_action = enable_disable_temp,
    .att_value.type = BT_SIZE8,
    .att_value.value.u64 = 0x0,
    .att_uuid.data = UUID_TEMP_ED,
    .properties.write = 1,
    .properties.read = 1,
    .att_handle =0x000B,
  },
  &(attribute_t){ // PRIMARY SERVICE DECLARATION : HUMIDITY
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_SIZE128,
    .att_value.value.u128.data = UUID_HUMIDITY_SERVICE,
    .att_uuid.data = UUID_PRIMARY_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x000C,
  },
  &(attribute_t){ // CHAR DECLARATION : HUMIDITY DATA
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_CHARACTERISTIC,
    .att_value.value.u64 = 0x02030011AA,
    .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x000D,
  },
  &(attribute_t){ // HUMIDITY DATA
    .get_action = actualise_humidity,
    .set_action = NULL,
    .att_value.type = BT_SIZE32,
    .att_value.value.u32 = 0x0E,
    .att_uuid.data = UUID_HUMIDITY_DATA,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x000E,
  },
  &(attribute_t){ // CHAR DECLARATION : HUMIDITY ED
    .get_action = NULL,
    .set_action = NULL,
    .att_value.type = BT_CHARACTERISTIC,
    .att_value.value.u64 = 0x020B0012AA,
    .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
    .properties.write = 0,
    .properties.read = 1,
    .att_handle =0x000F,
  },
  &(attribute_t){ // HUMIDITY ED
    .get_action = NULL,
    .set_action = enable_disable_humidity,
    .att_value.type = BT_SIZE8,
    .att_value.value.u8 = 0x0,
    .att_uuid.data = UUID_HUMIDITY_ED,
    .properties.write = 1,
    .properties.read = 1,
    .att_handle =0x0011,
  },
  // &(attribute_t){ // PRIMARY SERVICE DECLARATION : BAROMETER
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value.type = BT_SIZE128,
  //   .att_value.value.u128.data = UUID_BAROMETER_SERVICE,
  //   .att_uuid.data = UUID_PRIMARY_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0012,
  // },
  // &(attribute_t){ // CHAR DECLARATION : BAROMETER DATA
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_CHARACTERISTIC,
  //     .value.u64 = 0x02030021AA,
  //   },
  //   .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0013,
  // },
  // &(attribute_t){ // BAROMETER DATA
  //   .get_action = actualise_barometer,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE32,
  //     .value.u32 = 0x0,
  //   },
  //   .att_uuid.data = UUID_BAROMETER_DATA,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0014,
  // },
  // &(attribute_t){ // CHAR DECLARATION : BAROMETER ED
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_CHARACTERISTIC,
  //     .value.u64 = 0x020B0022AA,
  //   },
  //   .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0015,
  // },
  // &(attribute_t){ // BAROMETER ED
  //   .get_action = no_action,
  //   .set_action = enable_disable_barometer,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE8,
  //     .value.u8 = 0x0,
  //   },
  //   .att_uuid.data = UUID_BAROMETER_ED,
  //   .properties.write = 1,
  //   .properties.read = 1,
  //   .att_handle =0x0016,
  // },
  // &(attribute_t){ // PRIMARY SERVICE DECLARATION : LUXOMETER
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE128,
  //     .value.u128.data = UUID_LUXOMETER_SERVICE,
  //   },
  //   .att_uuid.data = UUID_PRIMARY_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0017,
  // },
  // &(attribute_t){ // CHAR DECLARATION : LUXOMETER DATA
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_CHARACTERISTIC,
  //     .value.u64 = 0x02030031AA,
  //   },
  //   .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0018,
  // },
  // &(attribute_t){ // LUXOMETER DATA
  //   .get_action = actualise_luxometer,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE16,
  //     .value.u16 = 0x0,
  //   },
  //   .att_uuid.data = UUID_LUXOMETER_DATA,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0019,
  // },
  // &(attribute_t){ // CHAR DECLARATION : LUXOMETER ED
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_CHARACTERISTIC,
  //     .value.u64 = 0x020B0032AA,
  //   },
  //   .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x001A,
  // },
  // &(attribute_t){ // LUXOMETER ED
  //   .get_action = no_action,
  //   .set_action = enable_disable_luxometer,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE8,
  //     .value.u8 = 0x0,
  //   },
  //   .att_uuid.data = UUID_LUXOMETER_ED,
  //   .properties.write = 1,
  //   .properties.read = 1,
  //   .att_handle =0x001B,
  // },
  // &(attribute_t){ // PRIMARY SERVICE DECLARATION : MPU
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE128,
  //     .value.u128.data = UUID_MPU_SERVICE,
  //   },
  //   .att_uuid.data = UUID_PRIMARY_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x001C,
  // },
  // &(attribute_t){ // CHAR DECLARATION : MPU DATA
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_CHARACTERISTIC,
  //     .value.u64 = 0x02030041AA,
  //   },
  //   .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x001D,
  // },
  // &(attribute_t){ // MPU DATA
  //   .get_action = actualise_mpu,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZEMPU,
  //   },
  //   .att_uuid.data = UUID_MPU_DATA,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x001E,
  // },
  // &(attribute_t){ // CHAR DECLARATION : MPU ED
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_CHARACTERISTIC,
  //     .value.u64 = 0x020B0042AA,
  //   },
  //   .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x001F,
  // },
  // &(attribute_t){ // MPU ED
  //   .get_action = no_action,
  //   .set_action = enable_disable_mpu,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE8,
  //     .value.u8 = 0x0,
  //   },
  //   .att_uuid.data = UUID_MPU_ED,
  //   .properties.write = 1,
  //   .properties.read = 1,
  //   .att_handle =0x0020,
  // },
  // &(attribute_t){ // PRIMARY SERVICE DECLARATION : LED
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE128,
  //     .value.u128.data = UUID_LED_SERVICE,
  //   },
  //   .att_uuid.data = UUID_PRIMARY_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0021,
  // },
  // &(attribute_t){ // CHAR DECLARATION : LED DATA
  //   .get_action = no_action,
  //   .set_action = no_action,
  //   .att_value=&(bt_size_t){
  //     .type = BT_CHARACTERISTIC,
  //     .value.u64 = 0x02030051AA,
  //   },
  //   .att_uuid.data = UUID_CHARACTERISTIC_DECLARATION,
  //   .properties.write = 0,
  //   .properties.read = 1,
  //   .att_handle =0x0022,
  // },
  // &(attribute_t){ // LED DATA
  //   .get_action = no_action,
  //   .set_action = actualise_led,
  //   .att_value=&(bt_size_t){
  //     .type = BT_SIZE8,
  //   },
  //   .att_uuid.data = UUID_MPU_DATA,
  //   .properties.write = 1,
  //   .properties.read = 1,
  //   .att_handle =0x0023,
  // },
  NULL
};

/*---------------------------------------------------------------------------*/
static attribute_t *get_attribute(const uint16_t handle){
  for(uint16_t i=0; list_attr[i] != NULL; i++){
    if (list_attr[i]->att_handle == handle){
      return list_attr[i];
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
// static void register_new_att_value(bt_size_t *att_value, uint8_t *data){
//   uint8_t *payload = &data[3];
//   switch(att_value->type){
//     case BT_SIZE8 :
//       att_value->value.u8 = *payload;
//       break;
//     case BT_SIZE16 :
//       att_value->value.u16 = *(uint16_t *)payload;
//       break;
//     case BT_SIZE32 :
//       att_value->value.u32 = *(uint32_t *)payload;
//       break;
//       case BT_SIZE64 :
//         att_value->value.u64 = *(uint64_t *)payload;
//         break;
//     case BT_SIZE128 :
//       att_value->value.u128 = *(uint128_t *)payload;
//       break;
//     default :
//       break;
//   }
// }
/*---------------------------------------------------------------------------*/
uint8_t get_value(const uint16_t handle, bt_size_t ** const value_ptr){
  attribute_t *att;
  PRINTF("GET VALUE\n");
  att = get_attribute(handle);

  if (!att)
    return ATT_ECODE_ATTR_NOT_FOUND;


  if (!att->properties.read)
    return ATT_ECODE_READ_NOT_PERM;
  if(att->get_action == NULL){
    *value_ptr = &att->att_value;
    return SUCCESS;
  }

  if(att->get_action(*value_ptr) != SUCCESS)
    return ATT_ECODE_UNLIKELY;

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t set_value(const uint16_t handle, uint8_t *data, uint16_t len){
  attribute_t *att;
  PRINTF("SET VALUE\n");
  att = get_attribute(handle);

  if (!att)
    return ATT_ECODE_ATTR_NOT_FOUND;


  if (!att->properties.write)
    return ATT_ECODE_WRITE_NOT_PERM;

  if (len < WRITE_REQUEST_HEADER && len > WRITE_REQUEST_LEN_MAX)
    return ATT_ECODE_INVAL_ATTR_VALUE_LEN;

  // register_new_att_value(&att->att_value, &data[3]);
PRINTF("coucou");
  if(!att->set_action || att->set_action(data) != SUCCESS)
    return ATT_ECODE_UNLIKELY;
PRINTF("coucou");

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
static uint16_t get_group_end(const uint16_t handle, const uint128_t *uuid_to_match){
  uint16_t i;
  for(i=handle; list_attr[i] != NULL; i++){

    if (uuid_128_compare(list_attr[i]->att_uuid, *uuid_to_match) == 1){
      return list_attr[i-1]->att_handle;
    }

  }
  return list_attr[i-1]->att_handle;
}
/*---------------------------------------------------------------------------*/
static attribute_t *get_attribute_by_uuid(const uint16_t starting_handle, const uint128_t *uuid_to_match, const uint16_t ending_handle){
  attribute_t *att;

  for(uint16_t i=starting_handle; list_attr[i] != NULL && i < ending_handle; i++){
    att = get_attribute(i);

    if (!att)
      return NULL;

    if (uuid_128_compare(att->att_uuid, *uuid_to_match) == 1){
      return att;
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
static void fill_response_tab_group(attribute_t *att, const uint16_t ending_handle, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups, const uint128_t *uuid_to_match){
  uint8_t curr_size, type_previous_value;
  uint16_t group_end_handle;
  curr_size = 0;

  while((curr_size + att->att_value.type) < (ATT_MTU - GROUP_RESPONSE_HEADER)){
    /* Look for the end handle of the group */
    group_end_handle = get_group_end(att->att_handle, uuid_to_match);

    /* Copy start handle of current group */
    memcpy(response_table+curr_size, &att->att_handle, sizeof(att->att_handle));
    curr_size += sizeof(att->att_handle);

    /* Copy end handle of current group */
    memcpy(response_table+curr_size, &group_end_handle, sizeof(group_end_handle));
    curr_size += sizeof(group_end_handle);

    /* Copy value */
    memcpy(response_table+curr_size, &att->att_value.value, att->att_value.type);
    curr_size += att->att_value.type;
    // PRINTF("Handle 8 = 0x%X", list_attr[7]);
    /* count number of groups to send */
    (*num_of_groups)++;

    type_previous_value = att->att_value.type;
    att = get_attribute(group_end_handle+1);


    /* Check if next group is not null or contain other value type */
    if (                 (att == NULL)                                  // verrify if next attribute is null
                      || (att->att_value.type != type_previous_value)   // verrify if next attribute's value is different type
                      || (att->att_handle > ending_handle)              // verrify if next attribute exceed ending_handle
                      || !(att->properties.read)){                      // verrify if next attribute can't be read
      /* length of one group */
      *lenght_group = curr_size/(*num_of_groups);
      break;
    }
  }
  /* length of one group */
  *lenght_group = curr_size/(*num_of_groups);
}
/*---------------------------------------------------------------------------*/
static void fill_response_tab(attribute_t *att, const uint16_t ending_handle, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups, const uint128_t *uuid_to_match){
  uint8_t curr_size, type_previous_value;
  uint64_t swap;
  curr_size = 0;

  while((curr_size + att->att_value.type) < (ATT_MTU - GROUP_RESPONSE_HEADER)){
    /* Copy start handle of current group */
    memcpy(response_table+curr_size, &att->att_handle, sizeof(att->att_handle));
    curr_size += sizeof(att->att_handle);
    /* TODO: Change the swap system */
    /* Copy value */
    swap = swap40(att->att_value.value.u64);
    memcpy(response_table+curr_size, &swap, att->att_value.type);
    curr_size += att->att_value.type;

    /* count number of groups to send */
    (*num_of_groups)++;

    type_previous_value = att->att_value.type;
    att = get_attribute_by_uuid((att->att_handle)+1, uuid_to_match, ending_handle);


    /* Check if next group is not null or contain other value type */
    if (                 (att == NULL)                                  // verrify if next attribute is null
                      || (att->att_value.type != type_previous_value)   // verrify if next attribute's value is different type
                      || (att->att_handle > ending_handle)              // verrify if next attribute exceed ending_handle
                      || !(att->properties.read)){                      // verrify if next attribute can't be read
      /* length of one group */
      *lenght_group = curr_size/(*num_of_groups);
      break;
    }
  }
  /* length of one group */
  *lenght_group = curr_size/(*num_of_groups);
}
/*---------------------------------------------------------------------------*/
uint8_t fill_group_type_response_values(const uint16_t starting_handle, const uint16_t ending_handle, const uint128_t *uuid_to_match, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups){
  attribute_t *att_groupe_start;
  PRINTF("GET GROUPE\n");

  /* Change this to support other group type */
  if(uuid_128_compare(*uuid_to_match, uuid_16_to_128(PRIMARY_GROUP_TYPE)) == 0)
    return ATT_ECODE_UNSUPP_GRP_TYPE;

  /* check if attribute is not null */
  att_groupe_start = get_attribute_by_uuid(starting_handle, uuid_to_match, ending_handle);
  if(!att_groupe_start)
    return ATT_ECODE_ATTR_NOT_FOUND;

  /* Fill in table */
  fill_response_tab_group(att_groupe_start, ending_handle, response_table,lenght_group, num_of_groups, uuid_to_match);

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t fill_type_response_values(const uint16_t starting_handle, const uint16_t ending_handle, const uint128_t *uuid_to_match, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups){
  attribute_t *att_groupe_start;
  PRINTF("GET GROUPE\n");

  /* check if attribute is not null */
  att_groupe_start = get_attribute_by_uuid(starting_handle, uuid_to_match, ending_handle);
  if(!att_groupe_start)
    return ATT_ECODE_ATTR_NOT_FOUND;

  /* Fill in table */
  fill_response_tab(att_groupe_start, ending_handle, response_table,lenght_group, num_of_groups, uuid_to_match);

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
