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
#include <stdint.h>
#include "net/ble-att.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/profiles/gatt-sensors.h"


#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
/* L2CAP fragmentation buffers and utilities                                 */
typedef struct {
  /* ATT Service Data Unit (SDU) */
  uint8_t sdu[ATT_MTU];
  /* length of the ATT SDU */
  uint16_t sdu_length;
} att_buffer_t;
static att_buffer_t g_tx_buffer;

static uint16_t g_error_handle;
/*---------------------------------------------------------------------------*/
static void send(){
  /* TODO: Add ATT MTU control */

  memcpy(packetbuf_dataptr(), g_tx_buffer.sdu, g_tx_buffer.sdu_length);
  packetbuf_set_datalen(g_tx_buffer.sdu_length);

  NETSTACK_MAC.send(NULL, NULL);
}
/*---------------------------------------------------------------------------*/
static uint8_t prepare_mtu_resp(){
  /* Response code */
  g_tx_buffer.sdu[0] = ATT_MTU_RESPONSE;
  /* Server Rx MTU */
  g_tx_buffer.sdu[1] = ATT_MTU;
  g_tx_buffer.sdu[2] = 0x00;
  /* set sdu length */
  g_tx_buffer.sdu_length = 3;
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
/*
 MTU request packet
 +------------------------+
 | Opcode | Client Rx MTU |
 +------------------------+
 Opcode : 1 octet
 Client RX MTU : 2 octets

*/
static uint8_t mtu_handle(uint8_t *data){

  prepare_mtu_resp();
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
static void prepare_error_resp(uint8_t* opcode, uint8_t error){
  /* Response code */
  g_tx_buffer.sdu[0] = ATT_ERROR_RESPONSE;
  /* Operation asked */
  g_tx_buffer.sdu[1] = opcode[0];
  /* Attribute handle that generate an error */
  memcpy(&g_tx_buffer.sdu[2], &g_error_handle, sizeof(g_error_handle));
  /* Error code */
  g_tx_buffer.sdu[4] = error;
  /* set sdu length */
  g_tx_buffer.sdu_length = 5;
}
/*---------------------------------------------------------------------------*/
/* NOT TESTED */
const char *error(uint8_t status){
	switch (status)  {
	case ATT_ECODE_INVALID_HANDLE:
		return "Invalid handle";
	case ATT_ECODE_READ_NOT_PERM:
		return "Attribute can't be read";
	case ATT_ECODE_WRITE_NOT_PERM:
		return "Attribute can't be written";
	case ATT_ECODE_INVALID_PDU:
		return "Attribute PDU was invalid";
	case ATT_ECODE_AUTHENTICATION:
		return "Attribute requires authentication before read/write";
	case ATT_ECODE_REQ_NOT_SUPP:
		return "Server doesn't support the request received";
	case ATT_ECODE_INVALID_OFFSET:
		return "Offset past the end of the attribute";
	case ATT_ECODE_AUTHORIZATION:
		return "Attribute requires authorization before read/write";
	case ATT_ECODE_PREP_QUEUE_FULL:
		return "Too many prepare writes have been queued";
	case ATT_ECODE_ATTR_NOT_FOUND:
		return "No attribute found within the given range";
	case ATT_ECODE_ATTR_NOT_LONG:
		return "Attribute can't be read/written using Read Blob Req";
	case ATT_ECODE_INSUFF_ENCR_KEY_SIZE:
		return "Encryption Key Size is insufficient";
	case ATT_ECODE_INVAL_ATTR_VALUE_LEN:
		return "Attribute value length is invalid";
	case ATT_ECODE_UNLIKELY:
		return "Request attribute has encountered an unlikely error";
	case ATT_ECODE_INSUFF_ENC:
		return "Encryption required before read/write";
	case ATT_ECODE_UNSUPP_GRP_TYPE:
		return "Attribute type is not a supported grouping attribute";
	case ATT_ECODE_INSUFF_RESOURCES:
		return "Insufficient Resources to complete the request";
	case ATT_ECODE_IO:
		return "Internal application error: I/O";
	case ATT_ECODE_TIMEOUT:
		return "A timeout occured";
	case ATT_ECODE_ABORTED:
		return "The operation was aborted";
	default:
		return "Unexpected error code";
	}
}
/*---------------------------------------------------------------------------*/
/*
 Read request packet
 +-----------------------------------------------------------+
 | Opcode | Starting Handle | Ending Handle | Attribute Type |
 +-----------------------------------------------------------+
 Opcode : 1 octet
 Starting handle : 2 octets
 Ending handle : 2 octets
 Attribute type : 2 or 16 octets
*/
/* prepare read response */
static uint8_t prepare_read(const uint8_t *data){
  uint16_t handle;
  uint8_t error;
  /* Copy handle to read */
  memcpy(&handle, &data[1], 2);

  bt_size_t *value_ptr;
  error = get_value(handle, &value_ptr);

  if (error != SUCCESS){
    g_error_handle = handle;
    return error;
  }

    /* Prepare payload */
    /* Response code */
  g_tx_buffer.sdu[0] = ATT_READ_RESPONSE;
  /* copy value in sdu */
  if (value_ptr->type == BT_SIZE_STR){ //specific treatment if value is a string
    memcpy(&g_tx_buffer.sdu[1], &value_ptr->value, strlen(value_ptr->value.str));
    g_tx_buffer.sdu_length =  strlen(value_ptr->value.str)+1;
  } else{
    memcpy(&g_tx_buffer.sdu[1], &value_ptr->value, value_ptr->type);
    g_tx_buffer.sdu_length = value_ptr->type+1;
  }


  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
/*
 Write request packet
 +-------------------------+
 | Opcode | Handle | Value |
 +-------------------------+
 Opcode : 1 octet
 handle : 2 octets
 Value : 0 to ATT_MTU - 3
*/
static uint8_t prepare_write(uint8_t *data, const uint16_t len){
  uint16_t handle;
  uint8_t error;
  /* Copy handle to write */
  memcpy(&handle, &data[1], 2);

  error = set_value(handle, data, len);

  if (error != SUCCESS){
    g_error_handle = handle;
    return error;
  }
  /* Prepare payload */
  /* Response code */
  g_tx_buffer.sdu[0] = ATT_WRITE_RESPONSE;
  g_tx_buffer.sdu_length = 1;

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
static uint8_t parse_type_req(uint8_t *data, uint16_t *starting_handle, uint16_t *ending_handle, uint128_t *uuid_to_match, uint16_t len){
  uint16_t tmp_uuid_16;
  /* Copy starting handle */
  memcpy(starting_handle, &data[1], 2);
  /* Copy ending handle */
  memcpy(ending_handle, &data[3], 2);
  /* see spec Bluetooth v5 p2198 */
  if (starting_handle > ending_handle || starting_handle == NULL_HANDLE)
    return ATT_ECODE_INVALID_HANDLE;
  /* for group request if len < 8 a 16bits uuid is sent, 128bits otherwise */
  if (len < 8){
    /* copy uudi to match */
    memcpy(&tmp_uuid_16, &data[5], BT_SIZE16);
    *uuid_to_match = uuid_16_to_128(tmp_uuid_16);
  } else{
    /* copy uudi to match */
    memcpy(&(uuid_to_match), &data[5], BT_SIZE128);
  }
    /* copy uudi to match */

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
/*
 Read by Type request packet && Read by group Type request packet
 +-----------------------------------------------------------+
 | Opcode | Starting Handle | Ending Handle | Attribute Type |
 +-----------------------------------------------------------+
 Opcode : 1 octet
 Starting handle : 2 octets
 Ending handle : 2 octets
 Attribute type : 2 or 16 octets
*/
static uint8_t prepare_type(uint8_t *data, uint16_t len){
  PRINTF("READ BY TYPE\n");
  uint128_t uuid_to_match;
  uint16_t starting_handle, ending_handle;
  uint8_t error, num_of_groups,lenght_group, tab_response[ATT_MTU - GROUP_RESPONSE_HEADER], response_type;

  starting_handle = ending_handle = 0;

  error = parse_type_req(data, &starting_handle, &ending_handle, &uuid_to_match, len);
  if (error != SUCCESS){
    g_error_handle = starting_handle;
    return error;
  }

  num_of_groups = 0;
  lenght_group = 0;

  if (data[0] == ATT_READ_BY_GROUP_TYPE_REQUEST){
    response_type = ATT_READ_BY_GROUP_TYPE_RESPONSE;
    error = fill_group_type_response_values(starting_handle, ending_handle, &uuid_to_match, tab_response, &lenght_group, &num_of_groups);
  } else{
    response_type = ATT_READ_BY_TYPE_RESPONSE;
    error = fill_type_response_values(starting_handle, ending_handle, &uuid_to_match, tab_response, &lenght_group, &num_of_groups);
  }
  if (error != SUCCESS){
    g_error_handle = starting_handle;
    return error;
  }
  /* Prepare payload */
  /* Response code */
  g_tx_buffer.sdu[0] = response_type;
  g_tx_buffer.sdu[1] =lenght_group;
  memcpy(&g_tx_buffer.sdu[2], tab_response, num_of_groups*lenght_group);

  g_tx_buffer.sdu_length = (num_of_groups*lenght_group)+2;

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
static void input(void){
  uint8_t control = ATT_ECODE_REQ_NOT_SUPP;
  /* Clear error  handles */
  g_error_handle = 0x0;

  uint8_t *data = (uint8_t *)packetbuf_dataptr();
  uint16_t len = packetbuf_datalen();
  switch (data[0]){
    case ATT_ERROR_RESPONSE:
        PRINTF("%s", error(data[4]));
      break;

    case ATT_MTU_REQUEST:
      control = mtu_handle(data);
      break;

    case ATT_READ_REQUEST :
      control = prepare_read(data);
      break;

    case ATT_WRITE_REQUEST :
      control = prepare_write(data, len);
      break;

    case ATT_READ_BY_GROUP_TYPE_REQUEST: /* see spec v5 p2200 */
      //FALLTHROUGH

    case ATT_READ_BY_TYPE_REQUEST:
      control = prepare_type(data, len);
      break;

    default :
      control = ATT_ECODE_REQ_NOT_SUPP;
      break;
  }
  if (control != SUCCESS)
  {
    prepare_error_resp(data, control);
  }
  send();
}
/*---------------------------------------------------------------------------*/

static void init(void){
 // Not used for now
}
/*---------------------------------------------------------------------------*/
const struct network_driver gatt_driver ={
  .name = "gatt_driver",
  .init = init,
  .input = input,
};
