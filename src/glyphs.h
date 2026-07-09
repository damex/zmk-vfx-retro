/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <lvgl.h>

#define VFX_RETRO_BATTERY_LEVEL_MAX 4
#define VFX_RETRO_BATTERY_WIDTH 18
#define VFX_RETRO_BATTERY_HEIGHT 8
#define VFX_RETRO_SIGNAL_WIDTH 11
#define VFX_RETRO_SIGNAL_HEIGHT 8
#define VFX_RETRO_DIGIT_WIDTH 5
#define VFX_RETRO_DIGIT_HEIGHT 7
#define VFX_RETRO_MODIFIER_WIDTH 12
#define VFX_RETRO_MODIFIER_HEIGHT 8
#define VFX_RETRO_INDICATOR_WIDTH 8
#define VFX_RETRO_INDICATOR_HEIGHT 8
#define VFX_RETRO_CELL_BORDER 1

enum vfx_retro_modifier {
    VFX_RETRO_MODIFIER_CTRL,
    VFX_RETRO_MODIFIER_SHIFT,
    VFX_RETRO_MODIFIER_ALT,
    VFX_RETRO_MODIFIER_GUI,
};

enum vfx_retro_indicator {
    VFX_RETRO_INDICATOR_CAPS,
    VFX_RETRO_INDICATOR_NUM,
    VFX_RETRO_INDICATOR_SCROLL,
};

uint8_t vfx_retro_battery_level(uint8_t state_of_charge);

uint8_t vfx_retro_signal_level(int8_t rssi_dbm);

void vfx_retro_draw_battery(lv_obj_t *canvas,
                            lv_coord_t x,
                            lv_coord_t y,
                            uint8_t scale,
                            uint8_t level);

void vfx_retro_draw_signal(lv_obj_t *canvas,
                           lv_coord_t x,
                           lv_coord_t y,
                           uint8_t scale,
                           int8_t rssi_dbm);

void vfx_retro_draw_signal_off(lv_obj_t *canvas,
                               lv_coord_t x,
                               lv_coord_t y,
                               uint8_t scale);

void vfx_retro_draw_number(lv_obj_t *canvas,
                           lv_coord_t right_x,
                           lv_coord_t y,
                           uint8_t scale,
                           uint8_t value);

void vfx_retro_draw_modifier(lv_obj_t *canvas,
                             lv_coord_t x,
                             lv_coord_t y,
                             uint8_t scale,
                             enum vfx_retro_modifier modifier,
                             bool active);

void vfx_retro_draw_indicator(lv_obj_t *canvas,
                              lv_coord_t x,
                              lv_coord_t y,
                              uint8_t scale,
                              enum vfx_retro_indicator indicator);
