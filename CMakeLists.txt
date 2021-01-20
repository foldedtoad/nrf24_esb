#
#
set(BOARD nrf51dk_nrf51422)
#set(BOARD nrf52840dk_nrf52840)

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf_sniff)

add_subdirectory(prx build/prx)
#add_subdirectory(ptx build/ptx)
add_subdirectory(lib/esb build/esb)

#zephyr_compile_options(-save-temps)
