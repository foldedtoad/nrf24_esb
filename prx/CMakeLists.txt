#   Configure and Make PRX side
#
set(BOARD nrf51dk_nrf51422)
#set(BOARD nrf52840dk_nrf52840)

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esb_prx)

FILE(GLOB app_sources ./*.c)
target_sources(app PRIVATE
  ${app_sources}
  )

add_subdirectory(../lib/esb build/esb)

# zephyr_compile_options(-save-temps)
