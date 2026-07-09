/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct vfx_retro_link {
    bool connected;
    bool searching;
    int8_t rssi_dbm;
    uint8_t channel;
};

struct vfx_retro_link vfx_retro_link_get(void);
