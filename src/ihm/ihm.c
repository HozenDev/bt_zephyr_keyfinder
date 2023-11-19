/** @file
 *  @brief Service Interface Homme-Machine
 */

/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <zephyr/init.h>
#include <zephyr/sys/__assert.h>
#include <stdbool.h>
#include <zephyr/types.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "ihm.h" /* ajout du header du service qui contient les fonctions de traitements des valeurs */

#define LOG_LEVEL CONFIG_BT_IHM_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ihm); /* ajoute le service au log */

/* -------------------------------------------------------- */
/*                     Service variables                    */
/* -------------------------------------------------------- */

static uint16_t button_duration; /* durée d'appui du bouton */
static uint8_t buzzer_level; /* niveau d'intensité du buzzer en % -> 0: aucun bruit, 100: son à fond */
static uint8_t led_state; /* état de la led -- 0: éteint, 1: allumé, 2..x..255: clignote x fois par sec */

/* -------------------------------------------------------- */
/*                       BUTTON features                    */
/* -------------------------------------------------------- */

/* callback pour la lecture de l'état du bouton */
static ssize_t read_button_duration(struct bt_conn *conn,
				    const struct bt_gatt_attr *attr, void *buf,
				    uint16_t len, uint16_t offset)
{
    uint16_t lvl16 = button_duration;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &lvl16,
			     sizeof(lvl16));
}

static ssize_t write_btnlvl(struct bt_conn *conn, 
			    const struct bt_gatt_attr *attr, 
			    const void *buf, uint16_t len,
			    uint16_t offset, uint8_t flags)
{
    memcpy(&button_duration, buf, sizeof(button_duration));
    LOG_INF("Button state %d\n", button_duration);
    return len;
}

/* -------------------------------------------------------- */
/*                       BUZZER features                    */
/* -------------------------------------------------------- */

/* callback pour la lecture de l'état du bouton */
static ssize_t read_buzzer_level(struct bt_conn *conn,
				 const struct bt_gatt_attr *attr, void *buf,
				 uint16_t len, uint16_t offset)
{
    uint8_t lvl8 = buzzer_level;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &lvl8,
			     sizeof(lvl8));
}

/* fonction de mise à jour si notification autorisé ou pas */
static void buzzer_level_ccc_cfg_changed(const struct bt_gatt_attr *attr,
					 uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("IHM:Buzzer Notifications %s", notif_enabled ? "enabled" : "disabled");
}

static ssize_t on_buzzer_receive(struct bt_conn *conn,
				 const struct bt_gatt_attr *attr,
				 const void *buf,
				 uint16_t len,
				 uint16_t offset,
				 uint8_t flags)
{
    const uint8_t * buffer = buf;
    
    printk("Received data, handle %d, conn %p, data: %0x", attr->handle, conn, len);
    for(uint8_t i = 0; i < len; i++){
        printk("%02X", buffer[i]);
        buzzer_level = buffer[i];
    }
    printk("\n");

    return len;
}

/* -------------------------------------------------------- */
/*                        LED features                      */
/* -------------------------------------------------------- */

/* callback pour la lecture de l'état du bouton */
static ssize_t read_led_state(struct bt_conn *conn,
			      const struct bt_gatt_attr *attr, void *buf,
			      uint16_t len, uint16_t offset)
{
    uint8_t lvl8 = led_state;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &lvl8,
			     sizeof(lvl8));
}

static ssize_t on_led_receive(struct bt_conn *conn,
			      const struct bt_gatt_attr *attr,
			      const void *buf,
			      uint16_t len,
			      uint16_t offset,
			      uint8_t flags)
{
    const uint8_t * buffer = buf;
    
    printk("Received data, handle %d, conn %p, data: %0x", attr->handle, conn, len);
    for(uint8_t i = 0; i < len; i++){
        printk("%02X", buffer[i]);
        led_state = (buffer[i] > 0) ? 1 : 0;
    }
    printk("\n");

    return len;
}

/* -------------------------------------------------------- */
/*                  GATT SERVICE Definition                 */
/* -------------------------------------------------------- */

/* configuration du service et ajout */
BT_GATT_SERVICE_DEFINE(ihm,
		       BT_GATT_PRIMARY_SERVICE(BT_UUID_IHM),
		       BT_GATT_CHARACTERISTIC(BT_UUID_IHM_BUZZER,
					      BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
					      BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
					      read_buzzer_level, on_buzzer_receive,
					      &buzzer_level),
		       BT_GATT_CUD(BT_CHAR_BUZZER_NAME, BT_GATT_PERM_READ),
		       BT_GATT_CCC(buzzer_level_ccc_cfg_changed,
				   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
		       BT_GATT_CHARACTERISTIC(BT_UUID_IHM_BUTTON,
					      BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
					      BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
					      read_button_duration, write_btnlvl,
					      &button_duration),
		       BT_GATT_CUD(BT_CHAR_BUTTON_NAME, BT_GATT_PERM_READ),
               BT_GATT_CCC(buzzer_level_ccc_cfg_changed,
				   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),         
		       BT_GATT_CHARACTERISTIC(BT_UUID_IHM_LED,
					      BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
					      BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
					      read_led_state, on_led_receive,
					      &led_state),
		       BT_GATT_CUD(BT_CHAR_LED_NAME, BT_GATT_PERM_READ),
               BT_GATT_CCC(buzzer_level_ccc_cfg_changed,
				   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    );

/* 
   le CCC est la configuration client, c'est à dire si il souhaite être notifié ou pas 
   -> pour les notify sinon pas besoin 
*/

/* -------------------------------------------------------- */
/*                 Initialisation function                  */
/* -------------------------------------------------------- */


static int ihm_init(void)
{
    button_duration = 0x00;
    buzzer_level = 0x00;
    led_state = 0x00;

    return 0;
}

/* -------------------------------------------------------- */
/*                   Treatment functions                    */
/* -------------------------------------------------------- */

uint8_t bt_ihm_get_buzzer_level(void)
{
    return buzzer_level;
}

int bt_ihm_set_buzzer_level(uint8_t level)
{
    int rc;

    if (level > 100U) {
	return -EINVAL;
    }

    buzzer_level = level;

    rc = bt_gatt_notify(NULL, &ihm.attrs[1], &level, sizeof(level));

    return rc == -ENOTCONN ? 0 : rc;
}

uint16_t bt_ihm_get_button_duration(void)
{
    return button_duration;
}

void bt_ihm_increment_button_duration(void)
{
    button_duration += 1;
    bt_gatt_notify(NULL, &ihm.attrs[4], &button_duration, sizeof(button_duration));
}

int bt_ihm_set_button_duration(uint16_t duration)
{
    int rc;

    button_duration = duration;

    rc = bt_gatt_notify(NULL, &ihm.attrs[4], &duration, sizeof(duration));

    return rc == -ENOTCONN ? 0 : rc;
}

uint8_t bt_ihm_get_led_state(void)
{
    return led_state;
}

int bt_ihm_set_led_state(uint8_t state)
{
    int rc;

    led_state = state;

    rc = bt_gatt_notify(NULL, &ihm.attrs[7], &state, sizeof(state));

    return rc == -ENOTCONN ? 0 : rc;
}

/* -------------------------------------------------------- */
/*                System service initialisation             */
/* -------------------------------------------------------- */

SYS_INIT(ihm_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
