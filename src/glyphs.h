/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#include <lvgl.h>

#define VFX_RETRO_BATTERY_LEVEL_MAX 4
#define VFX_RETRO_BATTERY_WIDTH 18
#define VFX_RETRO_BATTERY_HEIGHT 8
#define VFX_RETRO_SIGNAL_WIDTH 11
#define VFX_RETRO_SIGNAL_HEIGHT 8
#define VFX_RETRO_DIGIT_WIDTH 5
#define VFX_RETRO_DIGIT_HEIGHT 7

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
