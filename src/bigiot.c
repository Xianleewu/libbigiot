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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "pthread.h"
#include "bigiot.h"
#include "connection.h"

#define GETTIME_MESSAGE         "{\"M\":\"time\",\"F\":\"Y-m-d H:i:s\"}\n"
#define LOGIN_MESSAGE           "{\"M\":\"checkin\",\"ID\":\"%d\",\"K\":\"%s\"}\n"
#define UPDATE_ANALOG           "{\"M\":\"update\",\"ID\":\"%d\",\"V\":{\"%d\":\"%0.5f\"}}\n"
#define UPDATE_INTEGER          "{\"M\":\"update\",\"ID\":\"%d\",\"V\":{\"%d\":\"%d\"}}\n"
#define UPDATE_GPS              "{\"M\":\"update\",\"ID\":\"%d\",\"V\":{\"%d\":\"%0.5f,%0.5f\"}}\n"
#define CHECK_IS_ONLINE         "{\"M\":\"isOL\",\"ID\":[\"xx1\",...]}\n"
#define CHECK_ONLINE            "{\"M\":\"status\"}\n"
#define HEART_BEAT              "{\"M\":\"beat\"}\n"
#define HEART_BEAT_INTERVAL     4
#define CHECK_ONLINE_INTERVAL   20

int bigiot_login(bigiot_connection* connection, bigiot_device* device)
{
    char out_buf[MAX_DATA_SIZE] = {0};

    snprintf(out_buf, MAX_DATA_SIZE, LOGIN_MESSAGE, device->device_id, device->device_key);
    if( 0 > send(connection->socketfd, out_buf, strlen(out_buf), 0)) {
        perror("send login info failed!\n");
        return -1;
    }

    return 0;
}

void* bigiot_login_thread(void* arg)
{
    bigiot_session* session = (bigiot_session*)arg;

    pthread_detach(pthread_self());

    while(1) {
        if(session->conn->is_connected && !session->conn->is_registered) {
            char out_buf[MAX_DATA_SIZE] = {0};

            snprintf(out_buf, MAX_DATA_SIZE, LOGIN_MESSAGE, session->device->device_id, session->device->device_key);
            if( 0 > send(session->conn->socketfd, out_buf, strlen(out_buf), 0)) {
                perror("send login info failed!\n");
                session->conn->is_registered = 0;
            }
        }
        sleep(4);
    }

    pthread_exit(NULL);
}

int bigiot_update_analog(int sockfd, uint32_t device_id, uint32_t interface_id, float value)
{
    char out_buf[MAX_DATA_SIZE] = {0};

    snprintf(out_buf, MAX_DATA_SIZE, UPDATE_ANALOG, device_id, interface_id, value);
    if( 0 > send(sockfd, out_buf, strlen(out_buf), 0)) {
        perror("send analog info failed!\n");
        return -1;
    }

    return 0;
}

int bigiot_update_integer(int sockfd, uint32_t device_id, uint32_t interface_id, int value)
{
    char out_buf[MAX_DATA_SIZE] = {0};

    snprintf(out_buf, MAX_DATA_SIZE, UPDATE_INTEGER, device_id, interface_id, (value > 0) ? 1:0);
    if( 0 > send(sockfd, out_buf, strlen(out_buf), 0)) {
        perror("send integer info failed!\n");
        return -1;
    }

    return 0;
}

int bigiot_update_gps(int sockfd, uint32_t device_id, uint32_t interface_id, float lng, float lat)
{
    char out_buf[MAX_DATA_SIZE] = {0};

    snprintf(out_buf, MAX_DATA_SIZE, UPDATE_GPS, device_id, interface_id, lng, lat);
    if( 0 > send(sockfd, out_buf, strlen(out_buf), 0)) {
        perror("send gps info failed!\n");
        return -1;
    }

    return 0;
}

int bitiot_heart_beat(int sockfd)
{
    char out_buf[MAX_DATA_SIZE] = {0};

    snprintf(out_buf, MAX_DATA_SIZE, HEART_BEAT);

    return send(sockfd, out_buf, strlen(out_buf), 0);
}

int bigiot_check_online(int sockfd)
{
    char out_buf[MAX_DATA_SIZE] = {0};

    snprintf(out_buf, MAX_DATA_SIZE, CHECK_ONLINE);

    return send(sockfd, out_buf, strlen(out_buf), 0);
}

void* bigiot_heart_beat_thread(void *arg)
{
    bigiot_connection* connection = (bigiot_connection*)arg;

    pthread_detach(pthread_self());
    while(1) {
        if(connection->is_connected) {
            if(0 > bitiot_heart_beat(connection->socketfd)) {
                connection->send_failed++;
                printf("send heart beat failed!\n");
            }
        }
        sleep(HEART_BEAT_INTERVAL);
    }

    pthread_exit(NULL);
}

void* bigiot_check_online_thread(void* arg)
{
    bigiot_connection* connection = (bigiot_connection*)arg;

    pthread_detach(pthread_self());

    while(1) {
        sleep(CHECK_ONLINE_INTERVAL);
        if(connection->is_connected) {
            if(0 > bigiot_check_online(connection->socketfd)) {
                connection->send_failed++;
            }
        }
    }
}
