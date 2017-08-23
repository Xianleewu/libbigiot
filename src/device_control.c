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
#include "device_control.h"
#include "bigiot_camera.h"

void on_play_func(void* arg)
{
    // TODO:
    bigiot_upload_picture();
    printf("called on play func!\n");
}

void on_pause_func(void* arg)
{
    // TODO:
    printf("called on pause func!\n");
}

void on_stop_func(void* arg)
{
    // TODO:
    printf("called on stop func!\n");
}

void on_left_func(void* arg)
{
    // TODO:
    printf("called on left func!\n");
}

device_control_func device_control_func_map[] = {
    on_play_func,
    on_pause_func,
    on_left_func
};

int check_device_control_id(device_control_cmd cmdid)
{
    int cmd_table_size = 0;

    cmd_table_size = sizeof(device_control_func_map)/sizeof(device_control_func);

    if(cmd_table_size < cmdid) {
        printf("no match function for cmd of id:%d\n", cmdid);
        return -1;
    }

    return 0;
}

int call_device_control_func(device_control_cmd cmdid, void* arg)
{
    if(0 != check_device_control_id(cmdid)) {
        return -1;
    }

    device_control_func_map[cmdid](arg);

    return 0;
}
