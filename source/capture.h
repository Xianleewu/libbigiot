#ifndef __JPEG_CAPTURE_H__
#define __JPEG_CAPTURE_H__

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
    uint8_t* start;
    size_t length;
} buffer_t;

typedef struct {
    int fd;
    uint32_t width;
    uint32_t height;
    size_t buffer_count;
    buffer_t* buffers;
    buffer_t head;
} camera_t;

camera_t* camera_open(const char * device, uint32_t width, uint32_t height);

void camera_init(camera_t* camera);

void camera_start(camera_t* camera);

void camera_stop(camera_t* camera);

void camera_finish(camera_t* camera);

void camera_close(camera_t* camera);

int camera_capture(camera_t* camera);

int camera_frame(camera_t* camera, struct timeval timeout);

void jpeg(FILE* dest, uint8_t* rgb, uint32_t width, uint32_t height, int quality);

uint8_t* yuyv2rgb(uint8_t* yuyv, uint32_t width, uint32_t height);

#endif
