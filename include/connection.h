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

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#define BIGIOT_IPADDR       "121.42.180.30"
#define BIGIOT_PORT         8181        /* the port client will be connecting to */
#define MAX_DATA_SIZE       (1024*4)    /* max number of bytes we can get at once */
#define CHECKIN_OK          "checkinok"
#define IPADDR_MAX_LEN      64

typedef struct _bigiot_connection bigiot_connection;

typedef int (*recieve_callback_func)(bigiot_connection*,char*,int);

struct _bigiot_connection {
    int socketfd;
    int send_failed;
    int port;
    char ipaddr[IPADDR_MAX_LEN];
    char is_connected;
    char is_registered;
    recieve_callback_func recv_callback;
};

int bigiot_connect_server(const char* ipaddr, int port);

/**
 * @brief       callback for handle data from bigiot server
 *
 * @param data  incoming data
 * @param len   incoming data length
 *
 * @return 0
 */
int bigiot_recieve_callback(bigiot_connection* connection, char* data, int len);

void* recieve_data_thread(void* arg);

int bigiot_connection_init(bigiot_connection* connection, recieve_callback_func recv_callback);

#endif
