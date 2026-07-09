# Copyright 2026 Roman Kuzmitskii (@damex)
# SPDX-License-Identifier: MIT

if(CONFIG_VFX_RETRO)
  zephyr_library()
  zephyr_library_include_directories(${APPLICATION_SOURCE_DIR}/include)
  zephyr_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
  zephyr_library_compile_options(
      -Werror
      -Wextra
      -Wshadow
      -Wformat=2
      -Wlogical-op
      -Wduplicated-cond
      -Wduplicated-branches
      -Wmissing-prototypes
      -Wjump-misses-init
      -Wwrite-strings
      -Winit-self
      -Wold-style-definition
      -fanalyzer
  )
  zephyr_library_sources(src/battery.c)
  zephyr_library_sources(src/glyphs.c)
  zephyr_library_sources(src/indicators.c)
  zephyr_library_sources(src/link.c)
  zephyr_library_sources(src/modifiers.c)
  zephyr_library_sources(src/render.c)
  zephyr_library_sources(src/vfx_retro.c)
  zephyr_library_sources_ifdef(CONFIG_VFX_RETRO_STYLE_PACMAN src/styles/pacman.c)
endif()
