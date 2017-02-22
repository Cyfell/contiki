/*
 * Copyright (c) 2016, Arthur Courtel
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

#define L2CAP_ATT_CHANNEL           0x04

#define ATT_MTU_REQUEST             0x02
//#define ATT_MTU_RESPONSE            0x03
#define ATT_MTU_RESPONSE_LEN        0x03

void send_mtu_resp(){

  uint8_t data[ATT_MTU_RESPONSE_LEN];

  data[0]= ATT_MTU_RESPONSE_LEN;

  /* Server Rx MTU */
  data[1]=0x17;
  data[2]=0x00;
  memcpy(packetbuf_dataptr(), data,ATT_MTU_RESPONSE_LEN);
  packetbuf_set_datalen(ATT_MTU_RESPONSE_LEN);
  //packetbuf_copyfrom((void *)data, 3);
  NETSTACK_MAC.send(NULL, NULL);
}


void input_att(void){

  uint8_t *data = (uint8_t *)packetbuf_dataptr();
  //uint8_t len = packetbuf_datalen();
  // while (--len>0){
  //   printf("Data : 0x%x", *data++);
  //
  // }/*
  switch (data[0]){
    case ATT_MTU_REQUEST:
      send_mtu_resp();
      break;
    default :
      printf("Opcode number 0x%x not available", data[1]);
      break;
  }
}

static void init(void){

}

const struct network_driver gatt_driver =
{
  "gatt_driver",
  .init = init,
  .input = input_att,
};
