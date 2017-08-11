#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "pthread.h"
#include "connection.h"
#include "bigiot.h"
#include "signal.h"

#define DEVICE_ID       2523
#define DEVICE_KEY      "787e3eca2"
#define INTERFACE_ID    2400

pthread_t checkonline_thread;
pthread_t heartbeat_thread;
pthread_t login_thread;

bigiot_connection connection;

void on_socket_closed(int arg)
{
    printf("Socket closeed!\n");
    connection.is_connected = 0;
    connection.is_registered = 0;
}

int main(int argc, char** argv)
{
    int sockfd = -1;
    float value = 1.23;
    bigiot_device mydevice;
    bigiot_session session;
    struct sigaction action;

    memset(&connection, 0, sizeof(bigiot_connection));
    memset(&mydevice, 0, sizeof(bigiot_device));

    if(argc > 2) {
        memcpy(connection.ipaddr, argv[1], strlen(argv[1]));
        connection.port = atoi(argv[2]);
    } else {
        printf("Using default server info\n");
        memcpy(connection.ipaddr, BIGIOT_IPADDR, strlen(BIGIOT_IPADDR));
        connection.port = BIGIOT_PORT;
    }

    mydevice.device_id = DEVICE_ID;
    memcpy(mydevice.device_key, DEVICE_KEY, sizeof(DEVICE_KEY));

    /* signal(SIGPIPE, on_socket_closed); */

    action.sa_handler = on_socket_closed;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);
    signal(SIGPIPE, SIG_IGN);
    sockfd = bigiot_connection_init(&connection, bigiot_recieve_callback);
    if(sockfd == -1) {
        exit(-1);
    }

    session.conn = &connection;
    session.device = &mydevice;

    if(0 != pthread_create(&login_thread, NULL, bigiot_login_thread, &session)) {
        printf("Create login thread failed!\n");
        exit(0);
    }

    if(0 != pthread_create(&checkonline_thread, NULL, bigiot_check_online_thread, &connection)) {
        printf("Create recieve data thread failed!\n");
        exit(0);
    }

    if(0 != pthread_create(&heartbeat_thread, NULL, bigiot_heart_beat_thread, &connection)) {
        printf("Create heart beat thread failed!\n");
        exit(0);
    }

    while(1) {
        bigiot_update_analog(connection.socketfd, DEVICE_ID, INTERFACE_ID, value);
        value += 0.4;
        if(value >= 120 ) {
            value = 1.23;
        }
        sleep(5);
    }

    return 0;
}
