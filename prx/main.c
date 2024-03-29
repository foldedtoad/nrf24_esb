/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/nrf_clock_control.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/irq.h>
#include <zephyr/logging/log.h>
#include <nrf.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

#include "esb.h"
#include "esb_utils.h"

LOG_MODULE_REGISTER(esb_prx, 3);

#define LED_ON 0
#define LED_OFF 1

static const struct device *const clock0 = DEVICE_DT_GET_ONE(nordic_nrf_clock);

static const struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios),
};

static struct esb_payload rx_payload;
static struct esb_payload tx_payload = ESB_CREATE_PAYLOAD(0,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17);

static int leds_init(void)
{
    if (!device_is_ready(leds[0].port)) {
        LOG_ERR("LEDs port not ready");
        return -ENODEV;
    }

    for (size_t i = 0; i < ARRAY_SIZE(leds); i++) {
        int err = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT);

        if (err) {
            LOG_ERR("Unable to configure LED%u, err %d.", i, err);
            return err;
        }
    }

    return 0;
}

static void leds_update(uint8_t value)
{
    bool led0_status = !(value % 8 > 0 && value % 8 <= 4);
    bool led1_status = !(value % 8 > 1 && value % 8 <= 5);
    bool led2_status = !(value % 8 > 2 && value % 8 <= 6);
    bool led3_status = !(value % 8 > 3);

    gpio_port_pins_t mask = BIT(leds[0].pin) | BIT(leds[1].pin) |
                            BIT(leds[2].pin) | BIT(leds[3].pin);

    gpio_port_value_t val = led0_status << leds[0].pin |
                            led1_status << leds[1].pin |
                            led2_status << leds[2].pin |
                            led3_status << leds[3].pin;

    (void)gpio_port_set_masked_raw(leds[0].port, mask, val);
}

void event_handler(struct esb_evt const *event)
{
    switch (event->evt_id) {
    case ESB_EVENT_TX_SUCCESS:
        LOG_INF("TX SUCCESS EVENT");
        break;
    case ESB_EVENT_TX_FAILED:
        LOG_INF("TX FAILED EVENT");
        break;
    case ESB_EVENT_RX_RECEIVED:
        if (esb_read_rx_payload(&rx_payload) == 0) {
            LOG_INF("Packet received, len %d : "
                "0x%02x, 0x%02x, 0x%02x, 0x%02x, "
                "0x%02x, 0x%02x, 0x%02x, 0x%02x",
                rx_payload.length, rx_payload.data[0],
                rx_payload.data[1], rx_payload.data[2],
                rx_payload.data[3], rx_payload.data[4],
                rx_payload.data[5], rx_payload.data[6],
                rx_payload.data[7]);

            leds_update(rx_payload.data[1]);
        } else {
            LOG_ERR("Error while reading rx packet");
        }
        break;
    }
}

int clocks_start(void)
{
    int err;

    /* Is clock available? */
    if (!device_is_ready(clock0)) {
        printk("%s: device not ready.\n", clock0->name);
        return 0;
    }

    err = clock_control_on(clock0, CLOCK_CONTROL_NRF_SUBSYS_HF);
    if (err && (err != -EINPROGRESS)) {
        LOG_ERR("HF clock start fail: %d", err);
        return err;
    }

    /* Block until clock is started.
     */
    while (clock_control_get_status(clock0, CLOCK_CONTROL_NRF_SUBSYS_HF) !=
        CLOCK_CONTROL_STATUS_ON) {

    }

    LOG_DBG("HF clock started");
    return 0;
}

int esb_initialize(void)
{
    int err;
    /* These are arbitrary default addresses. In end user products
     * different addresses should be used for each set of devices.
     */
#if 0
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8};
#else
    uint8_t base_addr_0[4] = {0x22, 0x33, 0x44, 0x55};
    uint8_t base_addr_1[4] = {0xBB, 0xCC, 0xDD, 0xEE};
    uint8_t addr_prefix[8] = {0x11, 0x11, 0x11, 0x11, 0xAA, 0xAA, 0xAA, 0xAA};
#endif

    struct esb_config config = ESB_DEFAULT_CONFIG;

    config.protocol = ESB_PROTOCOL_ESB_DPL;
    config.bitrate = ESB_BITRATE_2MBPS;
    config.mode = ESB_MODE_PRX;
    config.event_handler = event_handler;
    config.selective_auto_ack = false;

    err = esb_init(&config);
    if (err) {
        return err;
    }

    err = esb_set_base_address_0(base_addr_0);
    if (err) {
        return err;
    }

    err = esb_set_base_address_1(base_addr_1);
    if (err) {
        return err;
    }

    err = esb_set_prefixes(addr_prefix, ARRAY_SIZE(addr_prefix));
    if (err) {
        return err;
    }

    return 0;
}

void main(void)
{
    int err;

    LOG_INF("Enhanced ShockBurst prx sample");

    err = clocks_start();
    if (err) {
        return;
    }

    leds_init();

    err = esb_initialize();
    if (err) {
        LOG_ERR("ESB initialization failed, err %d", err);
        return;
    }

    LOG_INF("Initialization complete");

    show_addresses_from_register();

    err = esb_write_payload(&tx_payload);
    if (err) {
        LOG_ERR("Write payload, err %d", err);
        return;
    }

    LOG_INF("Setting up for packet receiption");

    esb_set_rf_channel(0);  // set frequency to 2.400GHz

    err = esb_start_rx();
    if (err) {
        LOG_ERR("RX setup failed, err %d", err);
        return;
    }

    LOG_INF("waiting for events");

    /* return to idle thread */
    return;
}

