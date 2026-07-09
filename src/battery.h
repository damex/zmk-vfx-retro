/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>

#include <zephyr/sys/util.h>

#if IS_ENABLED(CONFIG_ZMK_BATTERY_REPORTING)

#include <zmk/battery.h>

static inline int vfx_retro_battery_state_of_charge(void) {
    return zmk_battery_state_of_charge();
}

#else

static inline int vfx_retro_battery_state_of_charge(void) {
    return -1;
}

#endif /* CONFIG_ZMK_BATTERY_REPORTING */

bool vfx_retro_battery_is_charging(void);
