/* Copyright (C)
 * 2017 - Xianlee.Wu  xianleewu@163.com
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _BIG_IOT_H_
#define _BIG_IOT_H_

#include "stdint.h"
#include "connection.h"

#define BIGIOT_DEVICE_KEY_LEN  64

typedef struct _bigiot_device {
    uint32_t device_id;
    char device_key[BIGIOT_DEVICE_KEY_LEN];
} bigiot_device;

typedef struct _bigiot_session {
    bigiot_connection* conn;
    bigiot_device* device;
} bigiot_session;

/**
 * @brief login bigiot server
 *
 * @param sockfd tcp socket fd
 * @param device id
 * @param device key
 *
 * @return 0
 */
int bigiot_login(bigiot_connection* connection, bigiot_device* device);

/**
 * @brief send analog value to bigiot server
 *
 * @param sockfd tcp socket fd
 * @param device_id
 * @param interface id
 * @param value
 *
 * @return 0
 */
int bigiot_update_analog(int sockfd, uint32_t device_id, uint32_t interface_id, float value);

/**
 * @brief send integer value to bigiot server
 *
 * @param sockfd
 * @param device_id
 * @param interface id
 * @param value
 *
 * @return 0
 */
int bigiot_update_integer(int sockfd, uint32_t device_id, uint32_t interface_id, int value);

/**
 * @brief sed gps postion info to bigiot server
 *
 * @param sockfd
 * @param device_id
 * @param interface id
 * @param lng
 * @param lat
 *
 * @return 0
 */
int bigiot_update_gps(int sockfd, uint32_t device_id, uint32_t interface_id, float lng, float lat);

/**
 * @brief loop send heartbeat
 *
 * @param arg
 *
 * @return NULL
 */
void* bigiot_heart_beat_thread(void *arg);

/**
 * @brief       check device is online
 *
 * @param arg   sockfd
 *
 * @return      NULL
 */
void* bigiot_check_online_thread(void* arg);

void* bigiot_login_thread(void* arg);

#endif
