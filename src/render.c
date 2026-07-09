/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include "render.h"

#include <stdbool.h>
#include <stdint.h>

#include <lvgl.h>

#if defined(CONFIG_VFX_RETRO_FONT_UNSCII_16)
#define TEXT_FONT (&lv_font_unscii_16)
#else
#define TEXT_FONT (&lv_font_unscii_8)
#endif

static bool xbm_pixel_lit(const uint8_t *bits,
                          uint16_t bytes_per_row,
                          uint16_t row,
                          uint16_t column) {
    uint8_t row_byte = bits[row * bytes_per_row + column / 8];
    return ((row_byte >> (column % 8)) & 1) != 0;
}

void vfx_retro_fill_rect(lv_obj_t *canvas,
                         lv_coord_t x,
                         lv_coord_t y,
                         lv_coord_t width,
                         lv_coord_t height,
                         lv_color_t color) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = color;
    lv_area_t coords = {x, y, x + width - 1, y + height - 1};
    lv_draw_rect(&layer, &rect_dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}

void vfx_retro_draw_text(lv_obj_t *canvas, lv_coord_t y, const char *text) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.font = TEXT_FONT;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    label_dsc.text = text;
    lv_area_t coords = {0, y, CONFIG_VFX_RETRO_PANEL_HEIGHT - 1,
                        y + CONFIG_VFX_RETRO_PANEL_WIDTH};
    lv_draw_label(&layer, &label_dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);
}

void vfx_retro_draw_xbm(lv_obj_t *canvas,
                        lv_coord_t x,
                        lv_coord_t y,
                        uint8_t scale,
                        const uint8_t *bits,
                        uint16_t width,
                        uint16_t height,
                        lv_color_t color) {
    uint16_t bytes_per_row = (uint16_t)((width + 7) / 8);
    for (uint16_t row = 0; row < height; row++) {
        for (uint16_t column = 0; column < width; column++) {
            if (!xbm_pixel_lit(bits, bytes_per_row, row, column)) {
                continue;
            }
            vfx_retro_fill_rect(canvas,
                                x + column * scale,
                                y + row * scale,
                                scale,
                                scale,
                                color);
        }
    }
}
