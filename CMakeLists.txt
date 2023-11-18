# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(peripheral_hr)

FILE(GLOB ihm_app_sources src/ihm/*.c)
target_sources(app PRIVATE
  ${ihm_app_sources}
)

FILE(GLOB system_app_sources src/system/*.c)
target_sources(app PRIVATE
  ${system_app_sources}
)

target_sources(app PRIVATE src/main.c)

zephyr_library_include_directories(${ZEPHYR_BASE}/samples/bluetooth)
