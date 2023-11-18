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

#include "system.h" /* ajout du header du service qui contient les fonctions de traitements des valeurs */

#define LOG_LEVEL CONFIG_BT_SYSTEM_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(system); /* ajoute le service au log */

/* -------------------------------------------------------- */
/*                     Service variables                    */
/* -------------------------------------------------------- */

static char version_value[BT_SYSTEM_VERSION_STRING_VALUE_LENGTH]; /* valeur de la version */
static char debug_value[BT_SYSTEM_DEBUG_STRING_VALUE_LENGTH]; /* valeur de la version */
static char advertising_value[BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH]; /* valeur de la version */

/* -------------------------------------------------------- */
/*                       VERSION features                   */
/* -------------------------------------------------------- */

static ssize_t read_version_value(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr, void *buf,
			       uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
				strlen(attr->user_data));
}

/* -------------------------------------------------------- */
/*                       DEBUG features                     */
/* -------------------------------------------------------- */

static ssize_t read_debug_value(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr, void *buf,
			       uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
				strlen(attr->user_data));
}

/* -------------------------------------------------------- */
/*                   ADVERTISING features                   */
/* -------------------------------------------------------- */

static ssize_t read_advertising_value(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr, void *buf,
			       uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
				strlen(attr->user_data));
}

static void advertising_ccc_cfg_changed(const struct bt_gatt_attr *attr,
				       uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("SYSTEM:Advertising Notifications %s", notif_enabled ? "enabled" : "disabled");
}

/* -------------------------------------------------------- */
/*                  GATT SERVICE Definition                 */
/* -------------------------------------------------------- */

/* configuration du service et ajout */
BT_GATT_SERVICE_DEFINE(system,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_SYSTEM),
    BT_GATT_CHARACTERISTIC(BT_UUID_SYSTEM_ADVERTISING,
			       BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, read_advertising_value, NULL,
			       &advertising_value),
	BT_GATT_CUD(BT_CHAR_ADVERTISING_NAME, BT_GATT_PERM_READ),
	BT_GATT_CCC(advertising_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_SYSTEM_VERSION,
			       BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, read_version_value, NULL,
			       &version_value),
	BT_GATT_CUD(BT_CHAR_VERSION_NAME, BT_GATT_PERM_READ),
    BT_GATT_CHARACTERISTIC(BT_UUID_SYSTEM_DEBUG,
			       BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, read_debug_value, NULL,
			       &debug_value),    
	BT_GATT_CUD(BT_CHAR_DEBUG_NAME, BT_GATT_PERM_READ),       
);

/* 
le CCC est la configuration client, c'est à dire si il souhaite être notifié ou pas 
    -> pour les notify sinon pas besoin 
*/

/* -------------------------------------------------------- */
/*                 Initialisation function                  */
/* -------------------------------------------------------- */

static int system_init(void)
{
    strncpy(version_value, "1.0.0.0", BT_SYSTEM_VERSION_STRING_VALUE_LENGTH-1);
	strncpy(debug_value, "Hello World!", BT_SYSTEM_DEBUG_STRING_VALUE_LENGTH-1);
	strncpy(advertising_value, "keyFinder", BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH-1);
	
	version_value[BT_SYSTEM_VERSION_STRING_VALUE_LENGTH-1] = '\0';
	debug_value[BT_SYSTEM_DEBUG_STRING_VALUE_LENGTH-1] = '\0';
	advertising_value[BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH-1] = '\0';

	return 0;
}

/* -------------------------------------------------------- */
/*                   Treatment functions                    */
/* -------------------------------------------------------- */

int bt_system_advertising_notify(char value[BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH])
{
	int rc;

	strncpy(advertising_value, value, BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH-1);
	advertising_value[BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH-1] = '\0';

	rc = bt_gatt_notify(NULL, &system.attrs[1], value, sizeof(value[0])*BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH);

	return rc == -ENOTCONN ? 0 : rc;
}

/* -------------------------------------------------------- */
/*                System service initialisation             */
/* -------------------------------------------------------- */

SYS_INIT(system_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);