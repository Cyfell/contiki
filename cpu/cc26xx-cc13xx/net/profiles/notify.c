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
#include "notify.h"
#include "gatt-sensors.h"
#include "etimer.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "temp.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static att_buffer_t g_tx_buffer_notify;
#define NUM_NOTIF_MAX 5
#define ATT_CID       4

PROCESS(sensors_notify_process, "Sensors");

static uint16_t list_sensor_notifications[NUM_NOTIF_MAX];
/*---------------------------------------------------------------------------*/
static uint8_t tab_get_empty_case(){
  for (int i = 0; i < NUM_NOTIF_MAX; i++){
    if(list_sensor_notifications[i] == 0)
      return i;
  }
  return NUM_NOTIF_MAX+1; //ERROR NOT FOUND
}
/*---------------------------------------------------------------------------*/
static uint8_t find_sensor_notified(uint16_t sensor){
  for (int i = 0; i < NUM_NOTIF_MAX; i++){
    if(list_sensor_notifications[i] == sensor)
      return i;
  }
  return NUM_NOTIF_MAX+1;//ERROR NOT FOUND
}
/*---------------------------------------------------------------------------*/
uint8_t status_notify(){
  if (find_sensor_notified(g_current_att->att_value.value.u16) == 0){
    return 0;
  } else{
    return 1;
  }
}
/*---------------------------------------------------------------------------*/
uint8_t enable_notification(){
  PRINTF("ENABLE NOTIFY\n");
  int empty_case;
  if((empty_case = tab_get_empty_case()) > NUM_NOTIF_MAX)
    return ATT_ECODE_UNLIKELY;

  list_sensor_notifications[empty_case] = g_current_att->att_value.value.u16;
  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
uint8_t disable_notification(){
  PRINTF("DISABLE NOTIFY\n");
  int i;
  i = find_sensor_notified(g_current_att->att_value.value.u16);

  if (i == -1)
    return ATT_ECODE_UNLIKELY;

  list_sensor_notifications[i] = 0;

  return SUCCESS;
}
/*---------------------------------------------------------------------------*/
static void prepare_notification(uint16_t handle_to_notify, bt_size_t *sensor_value){
  g_tx_buffer_notify.sdu[0] = ATT_HANDLE_VALUE_NOTIFICATION;
  memcpy(&g_tx_buffer_notify.sdu[1], &handle_to_notify, sizeof(handle_to_notify));
  memcpy(&g_tx_buffer_notify.sdu[3], &sensor_value->value, sensor_value->type);
  /* set sdu length */
  g_tx_buffer_notify.sdu_length = LENGHT_ATT_HEADER_NOTIFICATION + sensor_value->type;
}
/*---------------------------------------------------------------------------*/
static void prepare_error_resp(uint16_t error_handle, uint8_t error){
  /* Response code */
  g_tx_buffer_notify.sdu[0] = ATT_ERROR_RESPONSE;
  /* Operation asked */
  g_tx_buffer_notify.sdu[1] = ATT_HANDLE_VALUE_NOTIFICATION;
  /* Attribute handle that generate an error */
  memcpy(&g_tx_buffer_notify.sdu[2], &error_handle, sizeof(error_handle));
  /* Error code */
  g_tx_buffer_notify.sdu[4] = error;
  /* set sdu length */
  g_tx_buffer_notify.sdu_length = 5;
}
/*---------------------------------------------------------------------------*/
static void send(){
  memcpy(packetbuf_dataptr(), g_tx_buffer_notify.sdu, g_tx_buffer_notify.sdu_length);
  packetbuf_set_datalen(g_tx_buffer_notify.sdu_length);
  packetbuf_set_attr(PACKETBUF_ATTR_CHANNEL, ATT_CID);
  NETSTACK_MAC.send(NULL, NULL);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensors_notify_process, ev, data)
{
  static struct etimer notify_timer ;
  bt_size_t sensor_value;
  uint16_t handle_to_notify;
  uint8_t error;
  PROCESS_BEGIN();
  etimer_set(&notify_timer, CLOCK_SECOND);

  while(1){
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&notify_timer));
    etimer_reset(&notify_timer);
    for (int i = 0; i < NUM_NOTIF_MAX; i++){
      if(list_sensor_notifications[i] != 0){
        PRINTF("SEND NOTIFY\n");
        handle_to_notify = list_sensor_notifications[i];
        PRINTF("handle : %X\n", handle_to_notify);

        error = get_value(handle_to_notify, &sensor_value);
        if (error != SUCCESS){
          prepare_error_resp(handle_to_notify, error);
        }else{
          prepare_notification(handle_to_notify, &sensor_value);
        }
        send();
      }
    }
  }
  PROCESS_END();
}
