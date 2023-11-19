/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
// #include <zephyr/bluetooth/services/bas.h>
// #include <zephyr/bluetooth/services/hrs.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <inttypes.h>

// #include "myservice.h"
#include "ihm/ihm.h"

#include "system/system.h"

#define SLEEP_TIME_MS	1 // temps 

/* -------------------------------------------------------- */
/*                       SERVICES config                    */
/* -------------------------------------------------------- */

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		  BT_UUID_16_ENCODE(BT_UUID_HRS_VAL),
		  BT_UUID_16_ENCODE(BT_UUID_BAS_VAL),
		  BT_UUID_16_ENCODE(BT_UUID_DIS_VAL),
		  BT_UUID_16_ENCODE(BT_UUID_IHM_VAL),
		  BT_UUID_16_ENCODE(BT_UUID_SYSTEM_VAL)) // add this line for each service you want add
};

/* -------------------------------------------------------- */
/*                       BUTTON LED init                    */
/* -------------------------------------------------------- */

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
static struct gpio_callback button_cb_data;

/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on the LED whenever the button is pressed.
 */
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,
						     {0});

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int button_init(void)
{
    int ret;

    if (!gpio_is_ready_dt(&button)) {
	printk("Error: button device %s is not ready\n",
	       button.port->name);
	return 0;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
	printk("Error %d: failed to configure %s pin %d\n",
	       ret, button.port->name, button.pin);
	return 0;
    }

    ret = gpio_pin_interrupt_configure_dt(&button,
					  GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
	printk("Error %d: failed to configure interrupt on %s pin %d\n",
	       ret, button.port->name, button.pin);
	return 0;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin)); // définition callback à l'appui
    gpio_add_callback(button.port, &button_cb_data); // association de la callback au bouton
	
    printk("Set up button at %s pin %d\n", button.port->name, button.pin); 

    return 0;
}

int led_init(void)
{
    int ret;

    ret = device_is_ready(led.port);
    if (led.port && !ret) {
	printk("Error %d: LED device %s is not ready; ignoring it\n",
	       ret, led.port->name);
	led.port = NULL;
    }

    if (led.port) {
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret != 0) {
	    printk("Error %d: failed to configure LED device %s pin %d\n",
		   ret, led.port->name, led.pin);
	    led.port = NULL;
	} else {
	    printk("Set up LED at %s pin %d\n", led.port->name, led.pin);
	}
    }

    return 0;
}

/* -------------------------------------------------------- */
/*                                                          */
/* -------------------------------------------------------- */

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
	printk("Connection failed (err 0x%02x)\n", err);
    } else {
	printk("Connected\n");
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected (reason 0x%02x)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

static void bt_ready(void)
{
    int err;

    printk("Bluetooth initialized\n");

    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
	printk("Advertising failed to start (err %d)\n", err);
	return;
    }

    printk("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
    .cancel = auth_cancel,
};

/* -------------------------------------------------------- */
/*                       Notify functions                   */
/* -------------------------------------------------------- */

static void bas_notify(void)
{
    uint8_t battery_level = bt_bas_get_battery_level();

    battery_level--;

    if (!battery_level) {
	battery_level = 100U;
    }

    bt_bas_set_battery_level(battery_level);
}

static void hrs_notify(void)
{
    static uint8_t heartrate = 90U;

    /* Heartrate measurements simulation */
    heartrate++;
    if (heartrate == 160U) {
	heartrate = 90U;
    }

    bt_hrs_notify(heartrate);
}

static void buzzer_notify(void)
{
    uint8_t blevel = bt_ihm_get_buzzer_level();

    blevel += 10U;

    if (blevel >= 100U)
    {
	blevel = 0U;
    }

    bt_ihm_set_buzzer_level(blevel);
}

static void advertising_notify(void) 
{
    static char advalue[BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH];
    static uint8_t change = 0;

    if (change)
    {
	strncpy(advalue, "Oui", BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH-1);
    }
    else {
	strncpy(advalue, "Non", BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH-1);
    }

    change = !change;

    bt_system_advertising_notify(advalue);
}

/* -------------------------------------------------------- */
/*                       MAIN function                      */
/* -------------------------------------------------------- */

int main(void)
{
    int err;

    err = bt_enable(NULL);
    if (err) {
	printk("Bluetooth init failed (err %d)\n", err);
	return 0;
    }

    bt_ready();

    bt_conn_auth_cb_register(&auth_cb_display);

    /* Button & led initialisation */
    button_init();
    led_init();

    /* Implement notification. At the moment there is no suitable way
     * of starting delayed work so we do it here
     */
    int val = 0;
    if (led.port) {
	while (1) {
	    /* Heartrate measurements simulation */
	    hrs_notify();

	    /* Battery level simulation */
	    bas_notify();

	    /* Advertising simulaton */
	    advertising_notify();

	    /* Manage LED and button */
	    val = gpio_pin_get_dt(&button); // get button value
	    /* printk("Button State: %d\n", val); */

	    if (val > 0) 
	    {
            bt_ihm_increment_button_duration();
	    }
	    else {
		    bt_ihm_set_button_duration(0);
	    }

	    /* maj affichage leds */
	    bt_ihm_set_led_state(bt_ihm_get_button_duration());
	    gpio_pin_set_dt(&led, bt_ihm_get_led_state());

	    // incrémente buzzer si bouton appuyé depuis plus d'une seconde
	    if (bt_ihm_get_button_duration() > 100) {
		buzzer_notify();
		bt_ihm_set_button_duration(1);
	    }

	    k_msleep(SLEEP_TIME_MS);
	}
    }
    return 0;
}
