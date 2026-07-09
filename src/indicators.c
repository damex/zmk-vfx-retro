/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include "indicators.h"

#include <zephyr/toolchain.h>

__weak uint8_t vfx_retro_hid_indicators_get(void) {
    return 0;
}
