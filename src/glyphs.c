/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include "glyphs.h"

#include <stdint.h>

#include <lvgl.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain.h>

#include "render.h"

#include "bitmaps/battery_0.xbm"
#include "bitmaps/battery_1.xbm"
#include "bitmaps/battery_2.xbm"
#include "bitmaps/battery_3.xbm"
#include "bitmaps/battery_4.xbm"
#include "bitmaps/digit_0.xbm"
#include "bitmaps/digit_1.xbm"
#include "bitmaps/digit_2.xbm"
#include "bitmaps/digit_3.xbm"
#include "bitmaps/digit_4.xbm"
#include "bitmaps/digit_5.xbm"
#include "bitmaps/digit_6.xbm"
#include "bitmaps/digit_7.xbm"
#include "bitmaps/digit_8.xbm"
#include "bitmaps/digit_9.xbm"
#include "bitmaps/indicator_caps.xbm"
#include "bitmaps/indicator_num.xbm"
#include "bitmaps/indicator_scroll.xbm"
#include "bitmaps/modifier_alt.xbm"
#include "bitmaps/modifier_ctrl.xbm"
#include "bitmaps/modifier_gui.xbm"
#include "bitmaps/modifier_shift.xbm"
#include "bitmaps/signal_0.xbm"
#include "bitmaps/signal_1.xbm"
#include "bitmaps/signal_2.xbm"
#include "bitmaps/signal_3.xbm"
#include "bitmaps/signal_4.xbm"
#include "bitmaps/signal_off.xbm"

BUILD_ASSERT(battery_0_width == VFX_RETRO_BATTERY_WIDTH &&
             battery_0_height == VFX_RETRO_BATTERY_HEIGHT,
             "battery bitmap dims drifted");
BUILD_ASSERT(signal_0_width == VFX_RETRO_SIGNAL_WIDTH &&
             signal_0_height == VFX_RETRO_SIGNAL_HEIGHT,
             "signal bitmap dims drifted");
BUILD_ASSERT(digit_0_width == VFX_RETRO_DIGIT_WIDTH &&
             digit_0_height == VFX_RETRO_DIGIT_HEIGHT,
             "digit bitmap dims drifted");
BUILD_ASSERT(modifier_ctrl_width == VFX_RETRO_MODIFIER_WIDTH &&
             modifier_ctrl_height == VFX_RETRO_MODIFIER_HEIGHT,
             "modifier bitmap dims drifted");
BUILD_ASSERT(indicator_caps_width == VFX_RETRO_INDICATOR_WIDTH &&
             indicator_caps_height == VFX_RETRO_INDICATOR_HEIGHT,
             "indicator bitmap dims drifted");

#define BATTERY_LEVEL_STEP (100 / VFX_RETRO_BATTERY_LEVEL_MAX)
#define BATTERY_LEVEL_ROUND (BATTERY_LEVEL_STEP / 2)

#define SIGNAL_LEVEL_4_DBM (-50)
#define SIGNAL_LEVEL_3_DBM (-60)
#define SIGNAL_LEVEL_2_DBM (-70)
#define SIGNAL_LEVEL_1_DBM (-80)

uint8_t vfx_retro_battery_level(uint8_t state_of_charge) {
    uint8_t level = (uint8_t)((state_of_charge + BATTERY_LEVEL_ROUND) /
                              BATTERY_LEVEL_STEP);
    return MIN(level, VFX_RETRO_BATTERY_LEVEL_MAX);
}

uint8_t vfx_retro_signal_level(int8_t rssi_dbm) {
    if (rssi_dbm >= SIGNAL_LEVEL_4_DBM) {
        return 4;
    }
    if (rssi_dbm >= SIGNAL_LEVEL_3_DBM) {
        return 3;
    }
    if (rssi_dbm >= SIGNAL_LEVEL_2_DBM) {
        return 2;
    }
    if (rssi_dbm >= SIGNAL_LEVEL_1_DBM) {
        return 1;
    }
    return 0;
}

void vfx_retro_draw_battery(lv_obj_t *canvas,
                            lv_coord_t x,
                            lv_coord_t y,
                            uint8_t scale,
                            uint8_t level) {
    static const uint8_t *const level_bits[] = {
        battery_0_bits, battery_1_bits, battery_2_bits, battery_3_bits,
        battery_4_bits,
    };
    uint8_t clamped_level = MIN(level, VFX_RETRO_BATTERY_LEVEL_MAX);
    vfx_retro_draw_xbm(canvas,
                       x,
                       y,
                       scale,
                       level_bits[clamped_level],
                       VFX_RETRO_BATTERY_WIDTH,
                       VFX_RETRO_BATTERY_HEIGHT,
                       lv_color_white());
}

void vfx_retro_draw_signal(lv_obj_t *canvas,
                           lv_coord_t x,
                           lv_coord_t y,
                           uint8_t scale,
                           int8_t rssi_dbm) {
    static const uint8_t *const level_bits[] = {
        signal_0_bits, signal_1_bits, signal_2_bits, signal_3_bits,
        signal_4_bits,
    };
    vfx_retro_draw_xbm(canvas,
                       x,
                       y,
                       scale,
                       level_bits[vfx_retro_signal_level(rssi_dbm)],
                       VFX_RETRO_SIGNAL_WIDTH,
                       VFX_RETRO_SIGNAL_HEIGHT,
                       lv_color_white());
}

void vfx_retro_draw_signal_off(lv_obj_t *canvas,
                               lv_coord_t x,
                               lv_coord_t y,
                               uint8_t scale) {
    vfx_retro_draw_xbm(canvas,
                       x,
                       y,
                       scale,
                       signal_off_bits,
                       VFX_RETRO_SIGNAL_WIDTH,
                       VFX_RETRO_SIGNAL_HEIGHT,
                       lv_color_white());
}

void vfx_retro_draw_number(lv_obj_t *canvas,
                           lv_coord_t right_x,
                           lv_coord_t y,
                           uint8_t scale,
                           uint8_t value) {
    static const uint8_t *const digit_bits[] = {
        digit_0_bits, digit_1_bits, digit_2_bits, digit_3_bits, digit_4_bits,
        digit_5_bits, digit_6_bits, digit_7_bits, digit_8_bits, digit_9_bits,
    };
    lv_coord_t x = right_x;
    do {
        x -= VFX_RETRO_DIGIT_WIDTH * scale;
        vfx_retro_draw_xbm(canvas,
                           x,
                           y,
                           scale,
                           digit_bits[value % 10],
                           VFX_RETRO_DIGIT_WIDTH,
                           VFX_RETRO_DIGIT_HEIGHT,
                           lv_color_white());
        value /= 10;
        x -= scale;
    } while (value > 0);
}

static void draw_inverted_cell(lv_obj_t *canvas,
                               lv_coord_t x,
                               lv_coord_t y,
                               uint8_t scale,
                               uint8_t width,
                               uint8_t height) {
    vfx_retro_fill_rect(canvas,
                        x - VFX_RETRO_CELL_BORDER * scale,
                        y - VFX_RETRO_CELL_BORDER * scale,
                        (width + 2 * VFX_RETRO_CELL_BORDER) * scale,
                        (height + 2 * VFX_RETRO_CELL_BORDER) * scale,
                        lv_color_white());
}

void vfx_retro_draw_modifier(lv_obj_t *canvas,
                             lv_coord_t x,
                             lv_coord_t y,
                             uint8_t scale,
                             enum vfx_retro_modifier modifier,
                             bool active) {
    static const uint8_t *const glyph_bits[] = {
        [VFX_RETRO_MODIFIER_CTRL] = modifier_ctrl_bits,
        [VFX_RETRO_MODIFIER_SHIFT] = modifier_shift_bits,
        [VFX_RETRO_MODIFIER_ALT] = modifier_alt_bits,
        [VFX_RETRO_MODIFIER_GUI] = modifier_gui_bits,
    };
    if (active) {
        draw_inverted_cell(canvas,
                           x,
                           y,
                           scale,
                           VFX_RETRO_MODIFIER_WIDTH,
                           VFX_RETRO_MODIFIER_HEIGHT);
    }
    vfx_retro_draw_xbm(canvas,
                       x,
                       y,
                       scale,
                       glyph_bits[modifier],
                       VFX_RETRO_MODIFIER_WIDTH,
                       VFX_RETRO_MODIFIER_HEIGHT,
                       active ? lv_color_black() : lv_color_white());
}

void vfx_retro_draw_indicator(lv_obj_t *canvas,
                              lv_coord_t x,
                              lv_coord_t y,
                              uint8_t scale,
                              enum vfx_retro_indicator indicator) {
    static const uint8_t *const glyph_bits[] = {
        [VFX_RETRO_INDICATOR_CAPS] = indicator_caps_bits,
        [VFX_RETRO_INDICATOR_NUM] = indicator_num_bits,
        [VFX_RETRO_INDICATOR_SCROLL] = indicator_scroll_bits,
    };
    draw_inverted_cell(canvas,
                       x,
                       y,
                       scale,
                       VFX_RETRO_INDICATOR_WIDTH,
                       VFX_RETRO_INDICATOR_HEIGHT);
    vfx_retro_draw_xbm(canvas,
                       x,
                       y,
                       scale,
                       glyph_bits[indicator],
                       VFX_RETRO_INDICATOR_WIDTH,
                       VFX_RETRO_INDICATOR_HEIGHT,
                       lv_color_black());
}
