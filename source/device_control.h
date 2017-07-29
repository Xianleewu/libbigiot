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

#ifndef _DEVICE_CONTROL_H_
#define _DEVICE_CONTROL_H_

#include "stdint.h"
#include "connection.h"

typedef enum _device_control_cmd {
    DEVICE_CONTROL_CMD_PLAY = 0,
    DEVICE_CONTROL_CMD_PAUSE,
    DEVICE_CONTROL_CMD_STOP,
    DEVICE_CONTROL_CMD_LEFT,
    DEVICE_CONTROL_CMD_RIGHT,
    DEVICE_CONTROL_CMD_UP,
    DEVICE_CONTROL_CMD_DOWN,
    DEVICE_CONTROL_CMD_MINUS,
    DEVICE_CONTROL_CMD_PLUS,
    DEVICE_CONTROL_CMD_OFFON,
    DEVICE_CONTROL_CMD_BACKWARD,
    DEVICE_CONTROL_CMD_FORWARD
} device_control_cmd;

typedef void (*device_control_func)(void*);

int call_device_control_func(device_control_cmd cmdid, void* arg);

#endif
