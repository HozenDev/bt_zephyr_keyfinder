/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOZEN_INCLUDE_BLUETOOTH_SERVICES_SYSTEM_H_
#define HOZEN_INCLUDE_BLUETOOTH_SERVICES_SYSTEM_H_

/**
 * @brief System service (SYSTEM)
 * @defgroup bt_system System service (SYSTEM)
 * @ingroup bluetooth
 * @{
 *
 * [Experimental] Users should note that the APIs can change
 * as a part of ongoing development.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* valeur du service */
#define BT_UUID_SYSTEM_VAL 0x1400
#define BT_UUID_SYSTEM BT_UUID_DECLARE_16(BT_UUID_SYSTEM_VAL)
#define BT_CHAR_SYSTEM_NAME "System"

/* valeurs des caractéristiques */
/* VERSION */
#define BT_UUID_SYSTEM_VERSION_VAL 0x1401
#define BT_UUID_SYSTEM_VERSION BT_UUID_DECLARE_16(BT_UUID_SYSTEM_VERSION_VAL)
#define BT_CHAR_VERSION_NAME "Version"
/* DEBUG */
#define BT_UUID_SYSTEM_DEBUG_VAL 0x1402
#define BT_UUID_SYSTEM_DEBUG BT_UUID_DECLARE_16(BT_UUID_SYSTEM_DEBUG_VAL)
#define BT_CHAR_DEBUG_NAME "Debug"
/* ADVERTISING */
#define BT_UUID_SYSTEM_ADVERTISING_VAL 0x1403
#define BT_UUID_SYSTEM_ADVERTISING BT_UUID_DECLARE_16(BT_UUID_SYSTEM_ADVERTISING_VAL)
#define BT_CHAR_ADVERTISING_NAME "Advertising"

#define BT_SYSTEM_VERSION_STRING_VALUE_LENGTH 16 /* taille de la chaîne de caractères de la valeur de la version en octets */
#define BT_SYSTEM_DEBUG_STRING_VALUE_LENGTH 50 /* 50 caractères pour debug */
#define BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH 20 /* 20 caractères pour l'advertising */

#define CONFIG_BT_SYSTEM_LOG_LEVEL 3 /* log level service value */

/** @brief Notify advertising.
 *
 * This will send a GATT notification to all current subscribers.
 *
 *  @param value The value of the advertising string.
 *
 *  @return Zero in case of success and error code in case of error.
 */
int bt_system_advertising_notify(char value[BT_SYSTEM_ADVERTISING_STRING_VALUE_LENGTH]);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* HOZEN_INCLUDE_BLUETOOTH_SERVICES_SYSTEM_H_ */