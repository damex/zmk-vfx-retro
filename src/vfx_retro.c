/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>

#include <lvgl.h>
#include <zephyr/toolchain.h>
#include <zmk/display/status_screen.h>

#include "battery.h"
#include "glyphs.h"
#include "link.h"

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

#define ICON_SCALE 2
#define EDGE_MARGIN 4
#define TOP_Y 10
#define CHANNEL_GAP 2
#define READ_WIDTH CONFIG_VFX_RETRO_PANEL_HEIGHT
#define BATTERY_X EDGE_MARGIN
#define SIGNAL_X (READ_WIDTH - VFX_RETRO_SIGNAL_WIDTH * ICON_SCALE - EDGE_MARGIN)
#define CHANNEL_RIGHT_X (READ_WIDTH - EDGE_MARGIN)
#define CHANNEL_Y (TOP_Y + VFX_RETRO_SIGNAL_HEIGHT * ICON_SCALE + CHANNEL_GAP)

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

static void draw_battery(int state_of_charge) {
    if (state_of_charge < 0) {
        return;
    }
    uint8_t level = vfx_retro_battery_level((uint8_t)state_of_charge);
    vfx_retro_draw_battery(read_canvas,
                           BATTERY_X,
                           TOP_Y,
                           ICON_SCALE,
                           level);
}

static void draw_link(struct vfx_retro_link link) {
    if (link.searching) {
        vfx_retro_draw_signal_off(read_canvas,
                                  SIGNAL_X,
                                  TOP_Y,
                                  ICON_SCALE);
        return;
    }
    vfx_retro_draw_signal(read_canvas,
                          SIGNAL_X,
                          TOP_Y,
                          ICON_SCALE,
                          link.rssi_dbm);
    vfx_retro_draw_number(read_canvas,
                          CHANNEL_RIGHT_X,
                          CHANNEL_Y,
                          ICON_SCALE,
                          link.channel);
}

static void draw_sprite(void) {
}

static void draw_modifiers(void) {
}

static void draw_indicators(void) {
}

static void refresh(lv_timer_t *timer) {
    ARG_UNUSED(timer);
    lv_canvas_fill_bg(read_canvas, lv_color_black(), LV_OPA_COVER);
    draw_battery(vfx_retro_battery_state_of_charge());
    draw_link(vfx_retro_link_get());
    draw_sprite();
    draw_modifiers();
    draw_indicators();
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
