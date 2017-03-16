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
#define ATTRIBUTE_NUMBER_MAX 8


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
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
attribute_t *list_attr[ATTRIBUTE_NUMBER_MAX+1];
attribute_t primary_declaration_generic_access_service, characteristic_declaration_device_name, device_name;
attribute_t primary_declaration_temp, characteristic_declaration_temp_data, temp_data, characteristic_declaration_temp_ed, temp_ed;
/*---------------------------------------------------------------------------*/
static void register_value_8(attribute_t *att, const uint8_t value){
  att->att_value.type = BT_SIZE8;
  att->att_value.value.u8 = value;
}
/*---------------------------------------------------------------------------*/
static void register_value_16(attribute_t *att, const uint16_t value){
  att->att_value.type = BT_SIZE16;
  att->att_value.value.u16 = value;
}
/*---------------------------------------------------------------------------*/
// static void register_value_32(attribute_t *att, const uint32_t value){
//   att->att_value.type = BT_SIZE32;
//   att->att_value.value.u32 = value;
// }
/*---------------------------------------------------------------------------*/
static void register_value_64(attribute_t *att, const uint64_t value){
  att->att_value.type = BT_SIZE64;
  att->att_value.value.u64 = value;
}
/*---------------------------------------------------------------------------*/
static void register_value_128(attribute_t *att, const uint128_t value){
  att->att_value.type = BT_SIZE128;
  att->att_value.value.u128 = value;
}
/*---------------------------------------------------------------------------*/
static void register_value_str(attribute_t *att, const char value[20]){
  att->att_value.type = BT_SIZE_STR;
  strncpy(att->att_value.value.str, value, BT_SIZE_STR);
  PRINTF("value : %s | str : %s", value, att->att_value.value.str);
}
/*---------------------------------------------------------------------------*/
static void register_att( attribute_t *att, const uint8_t handle, const uint128_t uuid, const uint8_t readable, const uint8_t writable, const void *get, const void *set){
  att->att_handle = handle;
  att->att_uuid.type = BT_SIZE128;
  att->att_uuid.value.u128 = uuid;
  att->properties.read = readable;
  att->properties.write = writable;
  att->get_action = get;
  att->set_action = set;
}
/*---------------------------------------------------------------------------*/
static uint8_t no_action(){
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
void register_ble_attribute(uint8_t type){
  switch(type){
    case GENERIC_ACCESS_SERVICE:
      register_att(&primary_declaration_generic_access_service, 0x0001, uuid_16_to_128(0x2800), 1, 0,no_action, no_action);
      register_value_16(&primary_declaration_generic_access_service, 0x1800);

      register_att(&characteristic_declaration_device_name, 0x0002, uuid_16_to_128(0x2803), 1, 0,no_action, no_action);
      register_value_64(&characteristic_declaration_device_name, 0x020300002A);

      register_att(&device_name, 0x0003, uuid_16_to_128(0x2A00), 1, 0,no_action, no_action);
      register_value_str(&device_name, "Sensortag"); // hex to string : Sensortag

      list_attr[index_attr++]=&primary_declaration_generic_access_service;
      list_attr[index_attr++]=&characteristic_declaration_device_name;
      list_attr[index_attr++]=&device_name;
      break;

    case TEMPERATURE:
      register_att(&primary_declaration_temp, 0x0004, uuid_16_to_128(0x2800), 1, 0,no_action, no_action);
      register_value_16(&primary_declaration_temp, 0x0200);

      register_att(&characteristic_declaration_temp_data, 0x0005, uuid_16_to_128(0x2803), 1, 0,no_action, no_action);
      register_value_64(&characteristic_declaration_temp_data, 0x02030000AA);

      register_att(&temp_data, 0x0006, uuid_16_to_128(0x0200), 1, 0, actualise_temp, no_action);
      register_value_16(&temp_data, 0x00);

      register_att(&characteristic_declaration_temp_ed, 0x0007, uuid_16_to_128(0x2803), 1, 0,no_action, no_action);
      register_value_64(&characteristic_declaration_temp_ed, 0x02030000AA);

      register_att(&temp_ed, 0x0008, uuid_16_to_128(0x0300), 1, 1, no_action, enable_disable);
      register_value_16(&temp_ed, 0x0);

      list_attr[index_attr++]=&primary_declaration_temp;
      list_attr[index_attr++]=&characteristic_declaration_temp_data;
      list_attr[index_attr++]=&temp_data;
      list_attr[index_attr++]=&characteristic_declaration_temp_ed;
      list_attr[index_attr++]=&temp_ed;
      list_attr[index_attr++]=NULL;
      break;
  }
}
/*---------------------------------------------------------------------------*/
static attribute_t *get_attribute(const uint16_t handle){
  for(uint16_t i=0; i<ATTRIBUTE_NUMBER_MAX; i++){
    if (list_attr[i]->att_handle == handle){
      return list_attr[i];
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
      case BT_SIZE64 :
        att_value->value.u64 = *(uint64_t *)payload;
        break;
    case BT_SIZE128 :
      att_value->value.u128 = *(uint128_t *)payload;
      break;
    // case BT_SIZE_STR :
    //   att_value->value.str = *(char *)payload;
    //   break;
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


  if (!att->properties.read)
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


  if (!att->properties.write)
    return ATT_ECODE_WRITE_NOT_PERM;

  register_new_att_value(&att->att_value, data);

  if(!att->set_action || att->set_action(&att->att_value) != SUCCESS)
    return ATT_ECODE_UNLIKELY;


  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
static uint16_t get_group_end(const uint16_t handle, const bt_size_t *uuid_to_match){
  for(uint16_t i=handle; i<ATTRIBUTE_NUMBER_MAX; i++){
    if (uuid_128_compare(list_attr[i]->att_uuid.value.u128, uuid_to_match->value.u128) == 1){
      return list_attr[i-1]->att_handle;
    }

  }
  return list_attr[ATTRIBUTE_NUMBER_MAX-1]->att_handle;
}
/*---------------------------------------------------------------------------*/
static attribute_t *get_group_att_start(const uint16_t starting_handle, const bt_size_t *uuid_to_match){
  attribute_t *att;

  for(uint16_t i=starting_handle; i<ATTRIBUTE_NUMBER_MAX; i++){
    att = get_attribute(i);

    if (!att)
      return NULL;

    if (uuid_128_compare(att->att_uuid.value.u128, uuid_to_match->value.u128) == 1){
      return att;
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
static void fill_group_param(attribute_t *att, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups, const bt_size_t *uuid_to_match){
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

    /* count number of groups to send */
    (*num_of_groups)++;

    type_previous_value = att->att_value.type;
    att = get_attribute(group_end_handle+1);


    /* Check if next group is not null or contain other value type */
    if ((att == NULL) || (att->att_value.type != type_previous_value )){
      /* length of one group */
      *lenght_group = curr_size/(*num_of_groups);
      break;
    }
  }
  /* length of one group */
  *lenght_group = curr_size/(*num_of_groups);
}
/*---------------------------------------------------------------------------*/
uint8_t get_group(const uint16_t starting_handle, const uint16_t ending_handle, const bt_size_t *uuid_to_match, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups){
  attribute_t *att_groupe_start;
  PRINTF("GET GROUPE\n");

  /* Change this to support other group type */
  if(uuid_128_compare(uuid_to_match->value.u128, uuid_16_to_128(PRIMARY_GROUP_TYPE)) == 0)
    return ATT_ECODE_UNSUPP_GRP_TYPE;

  /* check if attribute is not null */
  att_groupe_start = get_group_att_start(starting_handle, uuid_to_match);
  if(!att_groupe_start)
    return ATT_ECODE_ATTR_NOT_FOUND;

  /* Fill in table */
  fill_group_param(att_groupe_start, response_table,lenght_group, num_of_groups, uuid_to_match);


  return SUCCESS;

}
/*---------------------------------------------------------------------------*/
