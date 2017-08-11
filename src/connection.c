/* Copyright (C)
 * 2017 - Xianlee.Wu
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
#include <pthread.h>
#include "connection.h"
#include "device_control.h"

static pthread_t recieve_thread;
static pthread_t connect_thread;

int bigiot_connect_server(const char* ipaddr, int port)
{
    int sockfd = -1;
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */

    if (!ipaddr || port < 0) {
        printf("please cheking your server info!\n");
        return -1;
    }

    if ((he=gethostbyname(ipaddr)) == NULL) {  /* get the host info */
        printf("gethostbyname");
        return -1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("failed to create socket!\n");
        return -1;
    }

    their_addr.sin_family = AF_INET;      /* host byte order */
    their_addr.sin_port = htons(port);    /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

    if ((connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
        printf("failed to connect to server!\n");
        return -1;
    }

    printf("Created socket fd:%d\n", sockfd);

    return sockfd;
}

void* connection_service(void* arg)
{
    int sockfd = -1;
    bigiot_connection* connection = (bigiot_connection*)arg;

    pthread_detach(pthread_self());

    while(1) {
        if(!connection->is_connected || connection->send_failed > 3) {
            printf("lost connection!\n");
            connection->is_connected = 0;
            connection->is_registered = 0;

            if(connection->socketfd > 0) {
                shutdown(connection->socketfd, SHUT_RDWR);
                close(connection->socketfd);
                connection->socketfd = -1;
                sockfd = -1;
            }

            sockfd = bigiot_connect_server(connection->ipaddr, connection->port);

            if(sockfd > 0) {
                connection->socketfd = sockfd;
                connection->is_connected = 1;
                connection->send_failed = 0;
                printf("connection is ok!\n");
            }
        }
        sleep(3);
    }

    pthread_exit(NULL);
}

void* recieve_data_thread(void *arg)
{
    bigiot_connection* connection = (bigiot_connection*)arg;
    char recieve_buf[MAX_DATA_SIZE] = {0};

    pthread_detach(pthread_self());

    if(!connection) {
        return NULL;
    } else {
        printf("Recieve data on socket:%d\n", connection->socketfd);
    }

    while(1) {
        if(connection->is_connected) {
            int recv_len = 0;

            memset(recieve_buf, 0, MAX_DATA_SIZE);
            recv_len = recv(connection->socketfd, recieve_buf, MAX_DATA_SIZE, 0);

            if(recv_len < 0) {
                printf("Recieve data error!\n");
                connection->send_failed++;
            }

            printf("--->data from server:\n%s\n", recieve_buf);

            if(connection->recv_callback) {
                connection->recv_callback(connection, recieve_buf, recv_len);
            }
        } else {
            sleep(2);
        }
    }

    printf("%s, %d:Recieve Thread dead!\n", __FUNCTION__, __LINE__);

    pthread_exit(NULL);
}

int bigiot_recieve_callback(bigiot_connection* connection, char* data, int len)
{
    if(NULL == data || len <= 0) {
        printf("callback data is invalued!\n");
        return -1;
    }

    if(strstr(data, "checkinok")) {
        printf("Device register ok!\n");
        connection->is_connected = 1;
        connection->is_registered = 1;
    } else if(strstr(data, "connected")) {
        printf("Device connected but not register yet!\n");
        connection->is_connected = 1;
        connection->is_registered = 0;
    } else if(strstr(data, "checked")) {
        printf("Device is online!\n");
        connection->is_connected = 1;
        connection->is_registered = 0;
    } else if(strstr(data, "play")) {
        printf("got play cmd\n");
        call_device_control_func(DEVICE_CONTROL_CMD_PLAY, data);
    } else {
        printf("Can't handle this message yet!\n");
    }

    return 0;
}

int bigiot_connection_init(bigiot_connection* connection, recieve_callback_func recv_callback)
{
    if(NULL == recv_callback) {
        printf("Please set recieve callback!\n");
        return -1;
    }

    if(0 != pthread_create(&connect_thread, NULL, connection_service, connection)) {
        printf("Create connection service failed\n");
        return -1;
    }

    connection->is_registered = 0;
    connection->send_failed = 0;
    connection->recv_callback = recv_callback;

    if(0 != pthread_create(&recieve_thread, NULL, recieve_data_thread, connection)) {
        printf("Create recieve data thread failed!\n");
        close(connection->socketfd);
        return -1;
    }

    return 0;
}
