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

#include "uuid.h"
#include <string.h>
#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define BASE_UUID16_OFFSET	2
static void bt_uuid_to_string(const uint128_t *uuid, char *str, size_t n)
{
	unsigned int   data0;
	unsigned short data1;
	unsigned short data2;
	unsigned short data3;
	unsigned int   data4;
	unsigned short data5;
	const uint8_t *data;


	data = (uint8_t *) uuid;

	memcpy(&data0, &data[0], 4);
	memcpy(&data1, &data[4], 2);
	memcpy(&data2, &data[6], 2);
	memcpy(&data3, &data[8], 2);
	memcpy(&data4, &data[10], 4);
	memcpy(&data5, &data[14], 2);

	snprintf(str, n, "%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
				data0, data1,
				data2, data3,
				data4, data5);

}
static uint128_t bluetooth_base_uuid = {
	.data = {	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
			0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB }
};

inline uint128_t uuid_16_to_128(uint16_t uuid_16){
 uint128_t result;
 /* Set base uuid */
 result = bluetooth_base_uuid;
// printf("uuid : 0x%X\n", uuid_16);
 uuid_16 = swap16(uuid_16);

 // printf("uuid : 0x%X\n", uuid_16);
 memcpy(&result.data[BASE_UUID16_OFFSET], &uuid_16, sizeof(uuid_16));
 // char uuid[40];
 // size_t s = 40;
 // bt_uuid_to_string(&result, uuid, s);
 // printf("uuid converted : %s\n", uuid);
 return result;
}

inline uint16_t uuid_128_to_16(const uint128_t uuid_128){
 uint16_t result;

memcpy(&result, &uuid_128.data[BASE_UUID16_OFFSET], sizeof(result));
 // result = (uuid_128.data[2] << 8) + uuid_128.data[3];
result = swap16(result);
 // printf("uuid : %x\n", result);
 return result;
}
inline uint8_t uuid_128_compare(const uint128_t u1, const uint128_t u2){
	/* TEST*/
	// char uuid[40];
	// size_t s = 40;
	// bt_uuid_to_string(&u1, uuid, s);
	// PRINTF("uuid data base : %s\n", uuid);
	// bt_uuid_to_string(&u2, uuid, s);
	// PRINTF("uuid cherche : %s\n", uuid);
	/* /TEST */
	for (uint8_t i = 0; i < sizeof(uint128_t); i++){
		//PRINTF("u1 : 0x%X || u2 : 0x%X", u1.data[i], u2.data[i]);
		if (u1.data[i] != u2.data[i])
			return 0;
	}
	return 1;
}

inline uint128_t swap128(const uint128_t *input){
	uint128_t output;
	for(uint8_t i = 0; i < sizeof(uint128_t); i++){
		output.data[i] = input->data[sizeof(uint128_t)-i-1];
	}
	return output;
}
