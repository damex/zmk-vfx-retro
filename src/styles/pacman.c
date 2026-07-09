/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include <stdbool.h>
#include <stdint.h>

#include <lvgl.h>
#include <zephyr/toolchain.h>

#include "link.h"
#include "render.h"
#include "style.h"

#include "bitmaps/pacman_closed.xbm"
#include "bitmaps/pacman_open.xbm"
#include "bitmaps/pacman_sleep.xbm"

BUILD_ASSERT(pacman_closed_width == pacman_open_width &&
             pacman_closed_height == pacman_open_height,
             "pacman closed frame differs from open");
BUILD_ASSERT(pacman_sleep_width == pacman_open_width &&
             pacman_sleep_height == pacman_open_height,
             "pacman sleep frame differs from open");

#define SPRITE_SCALE 2
#define SPRITE_Y 68
#define SPRITE_WIDTH (pacman_open_width * SPRITE_SCALE)
#define TRAVEL_STEP 4

static lv_coord_t center_x(lv_coord_t width) {
    return (lv_coord_t)((width - SPRITE_WIDTH) / 2);
}

void vfx_retro_style_draw(lv_obj_t *canvas,
                          lv_coord_t width,
                          struct vfx_retro_link link,
                          uint32_t frame) {
    bool searching = link.searching;
    lv_coord_t travel = width - SPRITE_WIDTH;
    lv_coord_t x = searching
        ? (lv_coord_t)((frame * TRAVEL_STEP) % (travel + 1))
        : center_x(width);
    const uint8_t *bits = (searching && frame % 2 != 0)
        ? pacman_closed_bits
        : pacman_open_bits;
    vfx_retro_draw_xbm(canvas,
                       x,
                       SPRITE_Y,
                       SPRITE_SCALE,
                       bits,
                       pacman_open_width,
                       pacman_open_height,
                       lv_color_white());
}

void vfx_retro_style_draw_sleep(lv_obj_t *canvas, lv_coord_t width) {
    vfx_retro_draw_xbm(canvas,
                       center_x(width),
                       SPRITE_Y,
                       SPRITE_SCALE,
                       pacman_sleep_bits,
                       pacman_open_width,
                       pacman_open_height,
                       lv_color_white());
}
