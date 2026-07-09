/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#include <lvgl.h>

void vfx_retro_fill_rect(lv_obj_t *canvas,
                         lv_coord_t x,
                         lv_coord_t y,
                         lv_coord_t width,
                         lv_coord_t height,
                         lv_color_t color);

void vfx_retro_draw_text(lv_obj_t *canvas, lv_coord_t y, const char *text);

void vfx_retro_draw_xbm(lv_obj_t *canvas,
                        lv_coord_t x,
                        lv_coord_t y,
                        uint8_t scale,
                        const uint8_t *bits,
                        uint16_t width,
                        uint16_t height,
                        lv_color_t color);
