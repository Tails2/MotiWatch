/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic
 * Semiconductor ASA.Terms and conditions of usage are described in detail
 * in NORDIC SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $$
 */

/** @file
 *
 * @defgroup ble_sdk_app_bps_main main.c
 * @{
 * @ingroup ble_sdk_app_bps
 * @brief Blood Pressure Service Sample Application main file.
 *
 * This file contains the source code for a sample application using the Blood pressure service.
 * This file also contains the code for initializing and using the Battery Service and the Device
 * Information Service. Furthermore, it demonstrates the use of the @ref srvlib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_bps.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
//#include "sensorsim.h"
#include "softdevice_handler.h"
#include "app_timer.h"
#include "device_manager.h"
#include "pstorage.h"
#include "app_trace.h"
#include "bsp.h"
#include "I2C_Lib.h"   

#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                          /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define CENTRAL_LINK_COUNT                   0                                     /**<number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT                1                                     /**<number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define SEND_MEAS_BUTTON_ID                  0                                     /**< Button used for sending a measurement. */

#define DEVICE_NAME                     "MOTI_WATCH"                                /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "Tails"                      							 /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM                       "00001"                           				 /**< Model number. Will be passed to Device Information Service. */

#define MANUFACTURER_ID                 0x1122334455                               /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                   0x667788                                   /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */

#define APP_ADV_INTERVAL                40                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                        /**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER             0                                          /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                          /**< Size of timer operation queues. */


#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER) /**< Battery level measurement interval (ticks). */ //таймер обмена данными

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)           /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)          /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                          /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)            /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of indication) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER)/**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                          /**< Number of attempts before giving up the connection parameter negotiation. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)   /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define SEC_PARAM_BOND                  1                                          /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                          /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                       /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                          /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                          /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                         /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                 /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

/**@brief Structure for a simulated blood pressure measurment. An instance of this struct is
          filled out before sending a notification to the peer with ble_bps_measurement_send.
 */
//typedef struct bps_meas_sim_value_s
//{
//    ieee_float16_t systolic;
//    ieee_float16_t diastolic;
//    ieee_float16_t mean_arterial;
//    ieee_float16_t pulse_rate;
//} bps_meas_sim_value_t;


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

void battery_level_update(uint8_t battery_level);

void blood_pressure_measurement_send(ble_bps_meas_t simulated_meas);

void battery_level_meas_timeout_handler(void * p_context);

void timers_init(void);

void gap_params_init(void);

void on_bps_evt(ble_bps_t * p_bps, ble_bps_evt_t * p_evt);

void services_init(void);

void application_timers_start(void);

void on_conn_params_evt(ble_conn_params_evt_t * p_evt);

void conn_params_error_handler(uint32_t nrf_error);

void conn_params_init(void);

void sleep_mode_enter(void);

void on_adv_evt(ble_adv_evt_t ble_adv_evt);

void on_ble_evt(ble_evt_t * p_ble_evt);

void ble_evt_dispatch(ble_evt_t * p_ble_evt);

void sys_evt_dispatch(uint32_t sys_evt);

void ble_stack_init(void);

void bsp_event_handler(bsp_event_t event);

uint32_t device_manager_evt_handler(dm_handle_t const * p_handle,
                                           dm_event_t const  * p_event,
                                           ret_code_t        event_result);

void device_manager_init(bool erase_bonds);

void advertising_init(void);

void buttons_leds_init(bool * p_erase_bonds);

void BLE_power_manage(void);

void Ble_Init(void);

void Set_Blod_meas(ble_bps_meas_t meas);
																					 
void ble_Rtc0_init(softdevice_evt_schedule_func_t MyHendler);
