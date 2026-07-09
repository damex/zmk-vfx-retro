/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#include <lvgl.h>

#include "link.h"

void vfx_retro_style_draw(lv_obj_t *canvas,
                          lv_coord_t width,
                          struct vfx_retro_link link,
                          uint32_t frame);

void vfx_retro_style_draw_sleep(lv_obj_t *canvas, lv_coord_t width);
