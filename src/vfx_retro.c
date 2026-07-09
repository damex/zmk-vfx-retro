/*
 * Copyright 2026 Roman Kuzmitskii (@damex)
 * SPDX-License-Identifier: MIT
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/modifiers.h>
#include <lvgl.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain.h>
#include <zmk/display/status_screen.h>

#include "battery.h"
#include "glyphs.h"
#include "indicators.h"
#include "link.h"
#include "modifiers.h"
#include "style.h"

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
#define ELEMENT_GAP 2
#define READ_WIDTH CONFIG_VFX_RETRO_PANEL_HEIGHT
#define READ_HEIGHT CONFIG_VFX_RETRO_PANEL_WIDTH
#define MODIFIER_COLUMNS 2

#define BATTERY_X EDGE_MARGIN
#define SIGNAL_X (READ_WIDTH - VFX_RETRO_SIGNAL_WIDTH * ICON_SCALE - EDGE_MARGIN)
#define CHANNEL_RIGHT_X (READ_WIDTH - EDGE_MARGIN)
#define CHANNEL_Y (TOP_Y + VFX_RETRO_SIGNAL_HEIGHT * ICON_SCALE + ELEMENT_GAP)

#define MODIFIER_CELL_WIDTH                                                \
    ((VFX_RETRO_MODIFIER_WIDTH + 2 * VFX_RETRO_CELL_BORDER) * ICON_SCALE)
#define MODIFIER_CELL_HEIGHT                                               \
    ((VFX_RETRO_MODIFIER_HEIGHT + 2 * VFX_RETRO_CELL_BORDER) * ICON_SCALE)
#define MODIFIER_COLUMN_SPAN (MODIFIER_CELL_WIDTH + ELEMENT_GAP)
#define MODIFIER_ROW_SPAN (MODIFIER_CELL_HEIGHT + ELEMENT_GAP)
#define MODIFIER_GRID_WIDTH                                                \
    (MODIFIER_COLUMNS * MODIFIER_CELL_WIDTH + ELEMENT_GAP)
#define MODIFIER_LEFT_X                                                    \
    ((READ_WIDTH - MODIFIER_GRID_WIDTH) / 2 +                              \
     VFX_RETRO_CELL_BORDER * ICON_SCALE)
#define MODIFIER_RIGHT_X (MODIFIER_LEFT_X + MODIFIER_COLUMN_SPAN)
#define MODIFIER_LOWER_Y                                                   \
    (READ_HEIGHT - EDGE_MARGIN -                                          \
     (VFX_RETRO_MODIFIER_HEIGHT + VFX_RETRO_CELL_BORDER) * ICON_SCALE)
#define MODIFIER_UPPER_Y (MODIFIER_LOWER_Y - MODIFIER_ROW_SPAN)

#define INDICATOR_SLOT_SPAN                                               \
    ((VFX_RETRO_INDICATOR_WIDTH + 2 * VFX_RETRO_CELL_BORDER) * ICON_SCALE)
#define INDICATOR_X (EDGE_MARGIN + VFX_RETRO_CELL_BORDER * ICON_SCALE)
#define INDICATOR_Y                                                       \
    (CHANNEL_Y + VFX_RETRO_DIGIT_HEIGHT * ICON_SCALE + ELEMENT_GAP +      \
     VFX_RETRO_CELL_BORDER * ICON_SCALE)

static uint8_t read_buf[READ_BUF_SIZE];
static uint8_t panel_buf[PANEL_BUF_SIZE];
static lv_obj_t *read_canvas;
static lv_obj_t *panel_canvas;
static uint32_t frame;

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

static void draw_sprite(struct vfx_retro_link link) {
    vfx_retro_style_draw(read_canvas, READ_WIDTH, link, frame);
}

static void draw_modifiers(uint8_t modifiers) {
    static const struct {
        uint8_t mask;
        enum vfx_retro_modifier glyph;
    } cells[] = {
        {MOD_LCTL | MOD_RCTL, VFX_RETRO_MODIFIER_CTRL},
        {MOD_LSFT | MOD_RSFT, VFX_RETRO_MODIFIER_SHIFT},
        {MOD_LALT | MOD_RALT, VFX_RETRO_MODIFIER_ALT},
        {MOD_LGUI | MOD_RGUI, VFX_RETRO_MODIFIER_GUI},
    };
    for (size_t index = 0; index < ARRAY_SIZE(cells); index++) {
        bool right_column = (index % MODIFIER_COLUMNS) != 0;
        bool upper_row = (index / MODIFIER_COLUMNS) != 0;
        lv_coord_t x = right_column ? MODIFIER_RIGHT_X : MODIFIER_LEFT_X;
        lv_coord_t y = upper_row ? MODIFIER_UPPER_Y : MODIFIER_LOWER_Y;
        vfx_retro_draw_modifier(read_canvas,
                                x,
                                y,
                                ICON_SCALE,
                                cells[index].glyph,
                                (modifiers & cells[index].mask) != 0);
    }
}

static void draw_indicators(uint8_t indicators) {
    static const struct {
        uint8_t mask;
        enum vfx_retro_indicator glyph;
    } cells[] = {
        {BIT(HID_USAGE_LED_CAPS_LOCK - 1), VFX_RETRO_INDICATOR_CAPS},
        {BIT(HID_USAGE_LED_NUM_LOCK - 1), VFX_RETRO_INDICATOR_NUM},
        {BIT(HID_USAGE_LED_SCROLL_LOCK - 1), VFX_RETRO_INDICATOR_SCROLL},
    };
    for (size_t index = 0; index < ARRAY_SIZE(cells); index++) {
        if ((indicators & cells[index].mask) == 0) {
            continue;
        }
        vfx_retro_draw_indicator(read_canvas,
                                 INDICATOR_X + index * INDICATOR_SLOT_SPAN,
                                 INDICATOR_Y,
                                 ICON_SCALE,
                                 cells[index].glyph);
    }
}

static void refresh(lv_timer_t *timer) {
    ARG_UNUSED(timer);
    struct vfx_retro_link link = vfx_retro_link_get();

    lv_canvas_fill_bg(read_canvas, lv_color_black(), LV_OPA_COVER);
    draw_battery(vfx_retro_battery_state_of_charge());
    draw_link(link);
    draw_sprite(link);
    draw_modifiers(vfx_retro_hid_modifiers_get());
    draw_indicators(vfx_retro_hid_indicators_get());
    blit_read_to_panel();

    if (link.searching) {
        frame++;
    }
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
