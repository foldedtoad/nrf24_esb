/*
 * Copyright (c) 2023 Callender-Consulting, LLC
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrf.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

#include "esb.h"
#include "esb_utils.h"

LOG_MODULE_REGISTER(esb_utils, 3);

/*
 *  Show the TXA and RXA address as retreived from the RADIO
 *  registers.  NOTE: this addresses are originally in BIGENDIAN format.
 */
void show_addresses_from_register(void)
{
    union reg_union {
        uint32_t  as_uint32;
        uint8_t   as_bytes[4];
    } base, prefix;

    base.as_uint32 = NRF_RADIO->BASE0;
    prefix.as_uint32 = NRF_RADIO->PREFIX0;

    LOG_INF("TXA: 0x%2X, 0x%2X, 0x%2X, 0x%2X, 0x%2X",
        (unsigned int)prefix.as_bytes[0],
        (unsigned int)base.as_bytes[0],
        (unsigned int)base.as_bytes[1],
        (unsigned int)base.as_bytes[2],
        (unsigned int)base.as_bytes[3]);

    base.as_uint32 = NRF_RADIO->BASE1;
    prefix.as_uint32 = NRF_RADIO->PREFIX1;

    LOG_INF("RXA: 0x%2X, 0x%2X, 0x%2X, 0x%2X, 0x%2X",
        (unsigned int)prefix.as_bytes[1],
        (unsigned int)base.as_bytes[0],
        (unsigned int)base.as_bytes[1],
        (unsigned int)base.as_bytes[2],
        (unsigned int)base.as_bytes[3]);
}
