#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "capture.h"
#include "bigiot_camera.h"

#define API_KEY     "9f95cd713"
#define DEVICE_ID   2534
#define INPUT_ID    2406

int bigiot_photo_post(char* filename, char* key, int did, int inputid)
{
    char post_cmd[512] = {0};

    memset(post_cmd, 0, 512);

    snprintf(post_cmd, 512, "curl --request POST -F \"data=@%s\" --header \"API-KEY: %s\" https://www.bigiot.net/pubapi/uploadImg/did/%d/inputid/%d",
             filename, key, did, inputid);

    printf("do post:\n%s\n", post_cmd);

    return system(post_cmd);
}

int bigiot_upload_picture()
{
    char filename[] = {"/tmp/bigiot.jpg"};
    camera_t* camera = camera_open("/dev/video0", 352, 288);
    camera_init(camera);
    camera_start(camera);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    /* skip 5 frames for booting a cam */
    for (int i = 0; i < 5; i++) {
        camera_frame(camera, timeout);
    }
    camera_frame(camera, timeout);

    unsigned char* rgb = yuyv2rgb(camera->head.start, camera->width, camera->height);
    FILE* out = fopen(filename, "w");
    jpeg(out, rgb, camera->width, camera->height, 100);
    fclose(out);
    free(rgb);

    bigiot_photo_post(filename, API_KEY, DEVICE_ID, INPUT_ID);

    camera_stop(camera);
    camera_finish(camera);
    camera_close(camera);
    return 0;
}

