/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOZEN_INCLUDE_BLUETOOTH_SERVICES_MYSERVICE_H_
#define HOZEN_INCLUDE_BLUETOOTH_SERVICES_MYSERVICE_H_

/**
 * @brief My Service (BAS)
 * @defgroup bt_myservice My Service (BAS)
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

/* définition des différents services ainsi que leurs valeurs */
#define BT_UUID_MYSERVICE_VAL 0x1400
#define BT_UUID_MYSERVICE BT_UUID_DECLARE_16(BT_UUID_MYSERVICE_VAL)

#define BT_UUID_MYSERVICE_CARAC1_VAL 0x1401
#define BT_UUID_MYSERVICE_CARAC1 BT_UUID_DECLARE_16(BT_UUID_MYSERVICE_CARAC1_VAL)

/* Exemple of function */
/** @brief Read battery level value.
 *
 * Read the characteristic value of the battery level
 *
 *  @return The battery level in percent.
 */
// uint8_t bt_bas_get_battery_level(void);


#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* HOZEN_INCLUDE_BLUETOOTH_SERVICES_MYSERVICE_H_ */
