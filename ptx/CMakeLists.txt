# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(.)

#add_subdirectory(prx build/prx)

target_sources(app PRIVATE main.c)
