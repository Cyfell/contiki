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
#define ATTRIBUTE_NUMBER_MAX 3

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#include "net/ble-att.h"
#include "net/profiles/gatt-sensors.h"
#include <stddef.h>
/*---------------------------------------------------------------------------*/
attribute_t list_attr[ATTRIBUTE_NUMBER_MAX];
attribute_t primary_temp, temp, temp_ed;
/*---------------------------------------------------------------------------*/
static void register_att_uuid16_value16( attribute_t *att, uint8_t handle, uint16_t uuid, uint8_t readable, uint8_t writable, void *get, void *set, uint8_t value_type, uint16_t value){
  att->att_handle = handle;
  att->att_uuid.type = BT_SIZE16;
  att->att_uuid.value.u16 = uuid;
  att->att_readable = readable;
  att->att_writable = writable;
  att->get_action = get;
  att->set_action = set;
  att->att_value.type = value_type;
  att->att_value.value.u16 = value;
}
static uint8_t no_action(){return SUCCESS;}
/*---------------------------------------------------------------------------*/
void register_ble_attribute(const uint8_t type){
  switch(type){
    case TEMPERATURE:
      register_att_uuid16_value16(&primary_temp, 0x0001, 0x2800, 1, 0,no_action, no_action, BT_SIZE16, 0x0200);
      register_att_uuid16_value16(&temp, 0x0002, 0x0200, 1, 0, actualise_temp, no_action, BT_SIZE16 ,0);
      register_att_uuid16_value16(&temp_ed, 0x0003, 0x0300, 1, 1, no_action, enable_disable, BT_SIZE16, 0);
      list_attr[0]=primary_temp;
      list_attr[1]=temp;
      list_attr[2]=temp_ed;
      break;
  }
}
/*---------------------------------------------------------------------------*/
static attribute_t *get_attribute(const uint16_t handle){
  for(int i=0; i<ATTRIBUTE_NUMBER_MAX; i++){
    if (list_attr[i].att_handle == handle){
      return &list_attr[i];
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
static void register_new_att_value(bt_size_t *att_value, uint8_t *data){
  uint8_t *payload = &data[3];
  switch(att_value->type){
    case BT_SIZE8 :
      att_value->value.u8 = *payload;
      break;
    case BT_SIZE16 :
      att_value->value.u16 = *(uint16_t *)payload;
      break;
    case BT_SIZE32 :
      att_value->value.u32 = *(uint32_t *)payload;
      break;
    case BT_SIZE128 :
      att_value->value.u128 = *(uint128_t *)payload;
      break;
  }
}
/*---------------------------------------------------------------------------*/
uint8_t get_value(const uint16_t handle, bt_size_t **value_ptr){
  attribute_t *att;
  PRINTF("GET VALUE\n");
  PRINTF("handle : %d\n", handle);
  att = get_attribute(handle);

  if (!att)
    return ATT_ECODE_ATTR_NOT_FOUND;


  if (!att->att_readable)
    return ATT_ECODE_READ_NOT_PERM;

  if(!att->get_action || att->get_action(&att->att_value) != SUCCESS)
    return ATT_ECODE_UNLIKELY;

  *value_ptr = &att->att_value;
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t set_value(const uint16_t handle, uint8_t *data, uint16_t len){
  attribute_t *att;
  PRINTF("SET VALUE\n");
  att = get_attribute(handle);

  if (!att)
    return ATT_ECODE_ATTR_NOT_FOUND;


  if (!att->att_writable)
    return ATT_ECODE_READ_NOT_PERM;

  register_new_att_value(&att->att_value, data);

  if(!att->set_action || att->set_action(&att->att_value) != SUCCESS)
    return ATT_ECODE_UNLIKELY;


  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
