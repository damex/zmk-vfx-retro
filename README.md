# zmk-vfx-retro

Retro pixel-perfect status screen for ZMK, keyboard or dongle. Battery, link signal
and channel, held modifiers, host lock indicators, and a sprite that chomps across the
screen while the link hunts for its peer. Every glyph is a 1-bit bitmap blown up to
solid blocks, sharp at any size. Knows nothing about the transport underneath: link
and HID state arrive through weak hooks, and whatever split you run fills them in.

## Install

Add it to your `config/west.yml`:
```yaml
  remotes:
    - name: damex
      url-base: https://github.com/damex
  projects:
    - name: zmk-vfx-retro
      remote: damex
      revision: v0.1.0
```
Then `west update`. For a local checkout, build with
`-DZMK_EXTRA_MODULES=<path>/zmk-vfx-retro` instead.

## Configure

Board/shield conf. Enable the module, drop the stock status widget, give LVGL a
full-frame buffer (the memory LCD renders whole frames):
```conf
CONFIG_VFX_RETRO=y
CONFIG_NICE_VIEW_WIDGET_STATUS=n
CONFIG_LV_Z_FULL_REFRESH=y
CONFIG_LV_Z_VDB_SIZE=100
CONFIG_LV_DPI_DEF=161
```

Tunables (Kconfig, defaults shown):

| Option | Default | Notes |
|---|---|---|
| `VFX_RETRO_PANEL_WIDTH` | 160 | panel width, pixels |
| `VFX_RETRO_PANEL_HEIGHT` | 68 | panel height, pixels |
| `VFX_RETRO_REFRESH_MS` | 1000 | redraw period while static |
| `VFX_RETRO_ANIMATION_MS` | 250 | redraw period while animating |

## Link state

Dynamic data arrives through weak hooks. Default build shows a connected link, no
charging, no modifiers or indicators. A transport or board feeds real data by
defining the strong symbol:

| Hook | Feeds |
|---|---|
| `vfx_retro_link_get` | signal bars, channel, searching sprite |
| `vfx_retro_battery_is_charging` | battery charge sweep |
| `vfx_retro_hid_modifiers_get` | modifier row, HID modifier bit order |
| `vfx_retro_hid_indicators_get` | caps/num/scroll, `zmk_hid_indicators_t` bit order |

Battery level comes from ZMK directly (`CONFIG_ZMK_BATTERY_REPORTING`).

## Styles

`VFX_RETRO_STYLE` picks the sprite. A style is one `src/styles/<name>.c` defining
`vfx_retro_style_draw` and `vfx_retro_style_draw_sleep`, drawing within the width
it is handed. Add one with a source file plus a Kconfig option.

- `PACMAN`: chomps left-to-right while searching, parks center when linked, sleeps
  idle.

The sprite animates off a frame counter that advances while searching and holds
while connected, so a still screen costs no redraws.

## Font

`VFX_RETRO_FONT` picks the text font, bitmap only (smooth fonts alias on these
panels).

- `UNSCII_8`: small (default)
- `UNSCII_16`: large

## Limitations

Layout is tuned for 160x68. Panel dimensions are configurable, but element
positions are fixed for that geometry, so other sizes need layout work.

Provides `zmk_display_status_screen`, so it owns the status screen. Turn off the
stock widget (`NICE_VIEW_WIDGET_STATUS=n`) and run no other custom status screen.

Two L8 canvas buffers, one byte per pixel (160x68 is about 11 KB each).

Single display.

## License

This module is MIT.

| Dependency | License |
|---|---|
| ZMK | MIT |
| Zephyr | Apache-2.0 |
| LVGL | MIT |
