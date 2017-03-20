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
#ifndef GATT_SENSORS_H_
#define GATT_SENSORS_H_
/*---------------------------------------------------------------------------*/
#define TEMPERATURE 1
#define GENERIC_ACCESS_SERVICE 2
/*---------------------------------------------------------------------------*/
#include "net/att-database.h"
#include "net/ble-att.h"
#include "net/profiles/temp.h"
#include "net/profiles/humidity.h"
/*---------------------------------------------------------------------------*/
uint8_t get_value(const uint16_t handle, bt_size_t **value_ptr);
uint8_t set_value(const uint16_t handle, uint8_t *data, uint16_t len);
uint8_t fill_group_type_response_values(const uint16_t starting_handle, const uint16_t ending_handle, const bt_size_t *uuid_to_match, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups);
uint8_t fill_type_response_values(const uint16_t starting_handle, const uint16_t ending_handle, const bt_size_t *uuid_to_match, uint8_t *response_table, uint8_t *lenght_group, uint8_t *num_of_groups);
/*---------------------------------------------------------------------------*/
#endif //GATT_SENSORS_H_
