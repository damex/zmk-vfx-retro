/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include "battery.h"

#include <zephyr/toolchain.h>

__weak bool vfx_retro_battery_is_charging(void) {
    return false;
}
