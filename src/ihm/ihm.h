/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOZEN_INCLUDE_BLUETOOTH_SERVICES_IHM_H_
#define HOZEN_INCLUDE_BLUETOOTH_SERVICES_IHM_H_

/**
 * @brief Interface Homme-Machine service (IHM)
 * @defgroup bt_ihm Interface Homme-Machine service (IHM)
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
#define BT_UUID_IHM_VAL 0x1410
#define BT_UUID_IHM BT_UUID_DECLARE_16(BT_UUID_IHM_VAL)
#define BT_CHAR_IHM_NAME "Interface Homme-Machine"

/* valeurs des caractéristiques */
/* Button */
#define BT_UUID_IHM_BUTTON_VAL 0x1411
#define BT_UUID_IHM_BUTTON BT_UUID_DECLARE_16(BT_UUID_IHM_BUTTON_VAL)
#define BT_CHAR_BUTTON_NAME "Button"
/* Buzzer */
#define BT_UUID_IHM_BUZZER_VAL 0x1412
#define BT_UUID_IHM_BUZZER BT_UUID_DECLARE_16(BT_UUID_IHM_BUZZER_VAL)
#define BT_CHAR_BUZZER_NAME "Buzzer"
/* LED */
#define BT_UUID_IHM_LED_VAL 0x1413
#define BT_UUID_IHM_LED BT_UUID_DECLARE_16(BT_UUID_IHM_LED_VAL)
#define BT_CHAR_LED_NAME "Led"

#define CONFIG_BT_IHM_LOG_LEVEL 3 /* log level service value */

/** @brief Read buzzer level value.
 *
 * Read the characteristic value of the buzzer level
 *
 *  @return The buzzer level in percent.
 */
    uint8_t bt_ihm_get_buzzer_level(void);

/** @brief Update buzzer level value.
 *
 * Update the characteristic value of the buzzer level
 * This will send a GATT notification to all current subscribers.
 *
 *  @param level The buzzer level in percent.
 *
 *  @return Zero in case of success and error code in case of error.
 */
    int bt_ihm_set_buzzer_level(uint8_t level);

/** @brief Read led state value.
 *
 * Read the characteristic value of the led state
 *
 *  @return The led state.
 */
    uint8_t bt_ihm_get_led_state(void);

/** @brief Update led state value.
 *
 * Update the characteristic value of the led state
 * This will send a GATT notification to all current subscribers.
 *
 *  @param state The led state.
 *
 *  @return Zero in case of success and error code in case of error.
 */
    int bt_ihm_set_led_state(uint8_t state);
    
    /** @brief Read button duration value.
     *
     * Read the characteristic value of the button duration
     *
     *  @return The button duration in milliseconds.
     */
    uint16_t bt_ihm_get_button_duration(void);

/** @brief Update button duration value.
 *
 * Update the characteristic value of the button duration
 * This will send a GATT notification to all current subscribers.
 *
 *  @param duration The button duration in milliseconds.
 *
 *  @return Zero in case of success and error code in case of error.
 */
    int bt_ihm_set_button_duration(uint16_t duration);

/** @brief Increment button duration value.
 *
 * Increment the characteristic value of the button duration
 */
    void bt_ihm_increment_button_duration(void);
    
#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* HOZEN_INCLUDE_BLUETOOTH_SERVICES_IHM_H_ */
