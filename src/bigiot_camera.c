#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "capture.h"
#include "curl/curl.h"
#include "bigiot_camera.h"

#define MAX_URL_LEN     (512)
#define MAX_URL_HDR_LEN (128)
#define API_KEY         "9f95cd713"
#define DEVICE_ID       (2534)
#define INPUT_ID        (2406)

int bigiot_photo_upload(const char* filename, const char* key, int did, int inputid)
{
    struct curl_httppost *post = NULL;
    struct curl_httppost *last = NULL;
    struct curl_slist* chunk = NULL;
    char dst_url[MAX_URL_LEN] = {0};
    char usrl_header[MAX_URL_HDR_LEN] = {0};
    CURL *curl = NULL;
    CURLcode ret;

    snprintf(dst_url, MAX_URL_LEN, "https://www.bigiot.net/pubapi/uploadImg/did/%d/inputid/%d", did, inputid);
    snprintf(usrl_header, MAX_URL_HDR_LEN, "API-KEY: %s", key);

    curl = curl_easy_init();

    curl_formadd(&post,
            &last,
            CURLFORM_COPYNAME, "data",
            CURLFORM_FILE, filename,
            CURLFORM_END);

    chunk = curl_slist_append(chunk, usrl_header);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_URL, dst_url);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    ret = curl_easy_perform(curl);
    curl_formfree(post);
    curl_easy_cleanup(curl);

    return 0;
}

int bigiot_upload_cam(const char* device, int w, int h, const char* key,
        int did, int inputid)
{
    const char* filename = "/tmp/post.jpg";
    camera_t* camera = camera_open(device, w, h);
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

    bigiot_photo_upload(filename, key, did, inputid);

    camera_stop(camera);
    camera_finish(camera);
    camera_close(camera);

    return 0;
}

