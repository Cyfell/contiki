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

static att_buffer_t tx_buffer;

/*---------------------------------------------------------------------------*/
static void send(){
  /* TODO: Add ATT MTU controll */

  memcpy(packetbuf_dataptr(), tx_buffer.sdu, tx_buffer.sdu_length);
  packetbuf_set_datalen(tx_buffer.sdu_length);

  NETSTACK_MAC.send(NULL, NULL);
}
/*---------------------------------------------------------------------------*/
static void prepare_mtu_resp(){
  /* Response code */
  tx_buffer.sdu[0]= ATT_MTU_RESPONSE;
  /* Server Rx MTU */
  tx_buffer.sdu[1]=ATT_MTU;
  tx_buffer.sdu[2]=0x00;
  /* set sdu length */
  tx_buffer.sdu_length = 3;
}
/*---------------------------------------------------------------------------*/
static void prepare_error_resp(uint8_t* opcode, uint8_t error)
{
  /* Response code */
  tx_buffer.sdu[0] = ATT_ERROR_RESPONSE;
  /* Operation asked */
  tx_buffer.sdu[1] = opcode[0];
  /* Attribute handle that generate an error */
  tx_buffer.sdu[2] = 0x00;
  tx_buffer.sdu[3] = 0x00;
  /* Error code */
  tx_buffer.sdu[4] = error;
  /* set sdu length */
  tx_buffer.sdu_length = 5;
}
/*---------------------------------------------------------------------------*/
/* NOT TESTED */
const char *error(uint8_t status)
{
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
/* send read response */
static uint8_t prepare_read(const uint8_t *data){
  uint16_t handle;
  uint8_t error;
  /* Copy handle to read */
  memcpy(&handle, &data[1], 2);

  bt_size_t *value_ptr;
  error = get_value(handle, &value_ptr);

  if(error != SUCCESS)
    return error;

    /* Prepare payload */
    /* Response code */
  tx_buffer.sdu[0] = ATT_READ_RESPONSE;
  /* copy value in sdu */
  memcpy(&tx_buffer.sdu[1], &value_ptr->value, value_ptr->type);
  tx_buffer.sdu_length = value_ptr->type+1;

  return SUCCESS;
}

/*---------------------------------------------------------------------------*/
static uint8_t prepare_write(uint8_t *data, const uint16_t len){
  uint16_t handle;
  uint8_t error;
  /* Copy handle to write */
  memcpy(&handle, &data[1], 2);

  error = set_value(handle, data, len);

  if(error != SUCCESS)
    return error;
  /* Prepare payload */
  /* Response code */
  tx_buffer.sdu[0] = ATT_WRITE_RESPONSE;
  tx_buffer.sdu_length =1;

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
static void input(void){
  uint8_t control = ATT_ECODE_REQ_NOT_SUPP;

  uint8_t *data = (uint8_t *)packetbuf_dataptr();
  uint16_t len = packetbuf_datalen();

  switch (data[0]){
    case ATT_ERROR_RESPONSE:
    /* NOT TESTED */
        PRINTF("%s", error(data[4]));
      break;
    case ATT_MTU_REQUEST:
      prepare_mtu_resp();
      break;
    case ATT_READ_REQUEST :
      PRINTF("READ REQUEST\n");
      control = prepare_read(data);
      break;
    case ATT_WRITE_REQUEST :
    PRINTF("WRITE REQUEST\n");
      control = prepare_write(data, len);
      break;
    default :
      control = ATT_ECODE_REQ_NOT_SUPP;
      break;
  }
  if(control != SUCCESS)
  {
    prepare_error_resp(data, control);
  }
  send();
}
/*---------------------------------------------------------------------------*/
static void init(void)
{
  register_ble_attribute(TEMPERATURE);

  // bt_size_t test;
  // bt_size_t *ptr = test;
  // PRINTF("test : %d", get_value(1, ptr));

}
/*---------------------------------------------------------------------------*/
const struct network_driver gatt_driver =
{
  "gatt_driver",
  .init = init,
  .input = input,
};
