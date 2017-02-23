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

#include "ble-att.h"
#include <stdio.h>
#include "net/netstack.h"
#include "net/packetbuf.h"

#define L2CAP_ATT_CHANNEL                 0x04

/*---------------------------------------------------------------------------*/
/* opcode from spec v4.2 p 2201 and v5 p2211 */
#define ATT_ERROR_RESPONSE                0x01 /* IMPLEMENTED*/

#define ATT_MTU_REQUEST                   0x02 /* IMPLEMENTED*/
#define ATT_MTU_RESPONSE                  0x03 /* IMPLEMENTED*/

#define ATT_INFORMATION_REQUEST           0x04 /* NOT IMPLEMENTED*/
#define ATT_INFORMATION_RESPONSE          0x05 /* NOT IMPLEMENTED*/

#define ATT_FIND_INFO_BY_TYPE_REQUEST     0x06 /* NOT IMPLEMENTED*/
#define ATT_FIND_INFO_BY_TYPE_RESPONSE    0x07 /* NOT IMPLEMENTED*/

#define ATT_READ_BY_TYPE_REQUEST          0x08 /* NOT IMPLEMENTED*/
#define ATT_READ_BY_TYPE_RESPONSE         0x09 /* NOT IMPLEMENTED*/

#define ATT_READ_REQUEST                  0x0A /* NOT IMPLEMENTED*/
#define ATT_READ_RESPONSE                 0x0B /* NOT IMPLEMENTED*/

#define ATT_READ_BLOB_REQUEST             0x0C /* NOT IMPLEMENTED*/
#define ATT_READ_BLOB_RESPONSE            0x0D /* NOT IMPLEMENTED*/

#define ATT_READ_MULTIPLE_REQUEST         0x0E /* NOT IMPLEMENTED*/
#define ATT_READ_MULTIPLE_RESPONSE        0x0F /* NOT IMPLEMENTED*/

#define ATT_READ_BY_GROUP_TYPE_REQUEST    0x10 /* NOT IMPLEMENTED*/
#define ATT_READ_BY_GROUP_TYPE_RESPONSE   0x11 /* NOT IMPLEMENTED*/

#define ATT_WRITE_REQUEST                 0x12 /* NOT IMPLEMENTED*/
#define ATT_WRITE_RESPONSE                0x13 /* NOT IMPLEMENTED*/

#define ATT_WRITE_COMMAND_REQUEST         0x52 /* NOT IMPLEMENTED*/

#define ATT_PREPARE_WRITE_REQUEST         0x16 /* NOT IMPLEMENTED*/
#define ATT_PREPARE_WRITE_RESPONSE        0x17 /* NOT IMPLEMENTED*/

#define ATT_EXECUTE_WRITE_REQUEST         0x18 /* NOT IMPLEMENTED*/
#define ATT_EXECUTE_WRITE_RESPONSE        0x19 /* NOT IMPLEMENTED*/

#define ATT_HANDLE_VALUE_NOTIFICATION     0x1B /* NOT IMPLEMENTED*/
#define ATT_HANDLE_VALUE_INDICATION       0x1D /* NOT IMPLEMENTED*/
#define ATT_HANDLE_VALUE_CONFIRMATION     0x1E /* NOT IMPLEMENTED*/

#define ATT_SIGNED_WRITE_COMMAND          0xD2 /* NOT IMPLEMENTED*/
/*---------------------------------------------------------------------------*/

#define ATT_MTU_RESPONSE_LEN              0x03
#define ATT_ERROR_RESPONSE_LEN            0x05

#define ATT_MTU                           0x17

/*---------------------------------------------------------------------------*/
enum {
  INVALID_HANDLE=1,
  READ_NOT_PERMITTED,
  WRITE_NOT_PERMITTED,
  INVALID_PDU,
  INSUFFICIENT_AUTHENTICATION,
  REQUEST_NOT_SUPPORTED,
  INVALID_OFFSET,
  INSUFFICIENT_AUTHORIZATION,
  PREPARE_QUEUE_FULL,
  ATTRIBUTE_NOT_FOUND,
  ATTRIBUTE_NOT_LONG,
  INSUFFICIENT_ENCRYPTION_KEY_SIZE,
  INVALID_ATTRIBUTE_VALUE_LENGTH,
  UNLIKELY_ERROR,
  INSUFFICIENT_ENCRYPTION,
  UNSUPPORTED_GROUPE_TYPE,
  INSUFFICIENT_RESOURCES
};
//enum error_code error;

/*---------------------------------------------------------------------------*/
static void send_mtu_resp(){

  uint8_t data[ATT_MTU_RESPONSE_LEN];

  data[0]= ATT_MTU_RESPONSE;

  /* Server Rx MTU */
  data[1]=ATT_MTU;
  data[2]=0x00;

  memcpy(packetbuf_dataptr(), data,ATT_MTU_RESPONSE_LEN);
  packetbuf_set_datalen(ATT_MTU_RESPONSE_LEN);

  NETSTACK_MAC.send(NULL, NULL);
}
/*---------------------------------------------------------------------------*/
static void send_error_resp(uint8_t* opcode, uint8_t error)
{
  uint8_t data[ATT_ERROR_RESPONSE_LEN];

  data[0] = ATT_ERROR_RESPONSE;

  /* Server Rx MTU */
  data[1] = opcode[0];
  data[2] = 0x00;
  data[3] = 0x00;
  data[4] = error;

  memcpy(packetbuf_dataptr(), data, ATT_ERROR_RESPONSE_LEN);
  packetbuf_set_datalen(ATT_ERROR_RESPONSE_LEN);

  NETSTACK_MAC.send(NULL, NULL);
}

/*---------------------------------------------------------------------------*/
static void input(void){

  uint8_t *data = (uint8_t *)packetbuf_dataptr();
  //uint8_t len = packetbuf_datalen();
  switch (data[0]){
    case ATT_MTU_REQUEST:
      send_mtu_resp();
      break;
    default :

      send_error_resp(data, REQUEST_NOT_SUPPORTED);
      break;
  }
}

static void init(void){
  /* Nothing to do for now */
}

const struct network_driver gatt_driver =
{
  "gatt_driver",
  .init = init,
  .input = input,
};
