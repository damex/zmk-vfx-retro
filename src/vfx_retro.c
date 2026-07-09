/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>

#include <lvgl.h>
#include <zephyr/toolchain.h>
#include <zmk/display/status_screen.h>

#define READ_BUF_SIZE                                                      \
    LV_CANVAS_BUF_SIZE(CONFIG_VFX_RETRO_PANEL_HEIGHT,                      \
                       CONFIG_VFX_RETRO_PANEL_WIDTH,                       \
                       8,                                                  \
                       LV_DRAW_BUF_STRIDE_ALIGN)
#define PANEL_BUF_SIZE                                                     \
    LV_CANVAS_BUF_SIZE(CONFIG_VFX_RETRO_PANEL_WIDTH,                       \
                       CONFIG_VFX_RETRO_PANEL_HEIGHT,                      \
                       8,                                                  \
                       LV_DRAW_BUF_STRIDE_ALIGN)

static uint8_t read_buf[READ_BUF_SIZE];
static uint8_t panel_buf[PANEL_BUF_SIZE];
static lv_obj_t *read_canvas;
static lv_obj_t *panel_canvas;

static void blit_read_to_panel(void) {
    lv_draw_buf_t *read_drawbuf = lv_canvas_get_draw_buf(read_canvas);
    lv_draw_buf_t *panel_drawbuf = lv_canvas_get_draw_buf(panel_canvas);
    lv_draw_sw_rotate(read_drawbuf->data,
                      panel_drawbuf->data,
                      CONFIG_VFX_RETRO_PANEL_HEIGHT,
                      CONFIG_VFX_RETRO_PANEL_WIDTH,
                      read_drawbuf->header.stride,
                      panel_drawbuf->header.stride,
                      LV_DISPLAY_ROTATION_270,
                      LV_COLOR_FORMAT_L8);
    lv_obj_invalidate(panel_canvas);
}

static void refresh(lv_timer_t *timer) {
    ARG_UNUSED(timer);
    lv_canvas_fill_bg(read_canvas, lv_color_black(), LV_OPA_COVER);
    blit_read_to_panel();
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    read_canvas = lv_canvas_create(screen);
    lv_canvas_set_buffer(read_canvas,
                         read_buf,
                         CONFIG_VFX_RETRO_PANEL_HEIGHT,
                         CONFIG_VFX_RETRO_PANEL_WIDTH,
                         LV_COLOR_FORMAT_L8);
    lv_obj_add_flag(read_canvas, LV_OBJ_FLAG_HIDDEN);

    panel_canvas = lv_canvas_create(screen);
    lv_canvas_set_buffer(panel_canvas,
                         panel_buf,
                         CONFIG_VFX_RETRO_PANEL_WIDTH,
                         CONFIG_VFX_RETRO_PANEL_HEIGHT,
                         LV_COLOR_FORMAT_L8);
    lv_obj_align(panel_canvas, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_timer_t *timer = lv_timer_create(refresh,
                                        CONFIG_VFX_RETRO_REFRESH_MS,
                                        NULL);
    refresh(timer);
    return screen;
}
