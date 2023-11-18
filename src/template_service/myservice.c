/** @file
 *  @brief Test service
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
#include <zephyr/bluetooth/services/bas.h>

#include "myservice.h" /* ajout du header du service qui contient les fonctions de traitements des valeurs */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(myservice); /* ajoute le service au log */

static uint8_t carac1_read_value = 100U;

/* callback pour la lecture de la caractéristique 1 */
static ssize_t read_carac1(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr, void *buf,
			       uint16_t len, uint16_t offset)
{
	uint8_t lvl8 = carac1_read_value;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &lvl8,
				 sizeof(lvl8));
}

/* fonction de mise à jour si notification autorisé ou pas */
static void carac1_ccc_cfg_changed(const struct bt_gatt_attr *attr,
				       uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("MYSERVICE Notifications %s", notif_enabled ? "enabled" : "disabled");
}

/* configuration du service et ajout */
BT_GATT_SERVICE_DEFINE(myservice,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_MYSERVICE),
	BT_GATT_CHARACTERISTIC(BT_UUID_MYSERVICE_CARAC1,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, read_carac1, NULL,
			       &carac1_read_value),
	BT_GATT_CCC(carac1_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/* 
le CCC est la configuration client, c'est à dire si il souhaite être notifié ou pas 
    -> pour les notify sinon pas besoin 
*/

static int myservice_init(void)
{
    // init my service default variables values
	return 0;
}

SYS_INIT(myservice_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);