/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include "link.h"

#include <zephyr/toolchain.h>

__weak struct vfx_retro_link vfx_retro_link_get(void) {
    struct vfx_retro_link link = {
        .connected = true,
        .searching = false,
        .rssi_dbm = 0,
        .channel = 0,
    };
    return link;
}
