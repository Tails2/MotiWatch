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

//#include "I2C_Lib.h"   
#include "MyBLE.h"   
#include "My_Key_lib.h"

int testmeas=0;
static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;   /**< Handle of the current connection. */
static ble_bas_t                        m_bas;                                     /**< Structure used to identify the battery service. */
static ble_bps_t                        m_bps;                                     /**< Structure used to identify the blood pressure service. */

static bool                             m_bps_meas_ind_conf_pending = false;       /**< Flag to keep track of when an indication confirmation is pending. */

ble_bps_meas_t Blod_meas;

APP_TIMER_DEF(m_battery_timer_id);                                                 /**< Battery timer. */
static dm_application_instance_t        m_app_handle;                              /**< Application identifier allocated by device manager. */

static ble_uuid_t m_adv_uuids[] = {{BLE_UUID_BLOOD_PRESSURE_SERVICE,     BLE_UUID_TYPE_BLE},
                                   {BLE_UUID_BATTERY_SERVICE,            BLE_UUID_TYPE_BLE},
                                   {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}}; /**< Universally unique service identifiers. */

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for performing battery measurement and updating the Battery Level characteristic
 *        in Battery Service.
 */
void battery_level_update(uint8_t battery_level) 
{
    uint32_t err_code;
    err_code = ble_bas_battery_level_update(&m_bas, battery_level);
    if ((err_code != NRF_SUCCESS) &&
        (err_code != NRF_ERROR_INVALID_STATE) &&
        (err_code != BLE_ERROR_NO_TX_PACKETS) &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
        )
    {
        APP_ERROR_HANDLER(err_code);
    }
}



/**@brief Function for simulating and sending one Blood Pressure Measurement.
 */
void blood_pressure_measurement_send(ble_bps_meas_t meas)
{
    //ble_bps_meas_t simulated_meas;
    uint32_t       err_code;
    bool           is_indication_enabled;
		

    err_code = ble_bps_is_indication_enabled(&m_bps, &is_indication_enabled);
    APP_ERROR_CHECK(err_code);

    if (is_indication_enabled && !m_bps_meas_ind_conf_pending)
    {	
        err_code = ble_bps_measurement_send(&m_bps, &meas);
        switch (err_code)
        {
            case NRF_SUCCESS:
                // Measurement was successfully sent, wait for confirmation.
                m_bps_meas_ind_conf_pending = true;
                break;

            case NRF_ERROR_INVALID_STATE:
                // Ignore error.
                break;

            default:
                APP_ERROR_HANDLER(err_code);
                break;
        }
    }
}


ieee_float16_t Grad(uint8_t dt1,uint8_t dt2,uint16_t kalib){
	ieee_float16_t res;
	uint32_t in=((dt1*0xff)+dt2)*10;
	int x=(in/182)-kalib*10;
	if(x<-1800){x+=3600;}		
	if(x>=1800){x-=3600;}	
	res.mantissa=x;
	res.exponent=-1;
	return res;
}

ieee_float16_t Term(uint8_t dt1,uint8_t dt2){
	ieee_float16_t res;
	uint32_t in=(dt1*0xff)+dt2;
	int x=(in/340)+36;		
	res.mantissa=x;
	res.exponent=-1;
	return res;
}

void Set_Blod_meas(ble_bps_meas_t meas){
Blod_meas=meas;
}

/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
void battery_level_meas_timeout_handler(void * p_context)
{
	uint8_t data[14];		
	uint8_t i;
		for(i=0;i<14;i++){data[i]=i;}
		
		//if(get_my_Key()==1){data[0]=10;}
		//if(get_my_Key()==2){data[0]=30;}
	//	if(get_my_Key()==4){data[0]=50;}
	//Read_MPU6050(&data[0]);
		Blod_meas.blood_pressure_systolic=Grad(data[0],data[1],83);
		Blod_meas.blood_pressure_diastolic=Grad(data[2],data[3],2);			 
		Blod_meas.mean_arterial_pressure=Grad(data[4],data[5],353); 
		Blod_meas.blood_pressure_units_in_kpa=0x07;
		Blod_meas.pulse_rate=Term(data[6],data[7]);
		Blod_meas.blood_pressure_units_in_kpa=0x07;
		Blod_meas.measurement_status=0x08;
		Blod_meas.measurement_status_present=0x09;
		Blod_meas.pulse_rate_present=10;
		Blod_meas.time_stamp_present=11;
		Blod_meas.user_id_present=12;
		Blod_meas.user_id=3;
		Blod_meas.time_stamp.day=30;
		Blod_meas.time_stamp.month=8;
		Blod_meas.time_stamp.year=2019;
		Blod_meas.time_stamp.hours=17;
		Blod_meas.time_stamp.minutes=45;
		Blod_meas.time_stamp.seconds=10;
	
    UNUSED_PARAMETER(p_context);
    battery_level_update(85);
		blood_pressure_measurement_send(Blod_meas);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers.
    err_code = app_timer_create(&m_battery_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}





/**@brief Function for handling the Blood Pressure Service events.
 *
 * @details This function will be called for all Blood Pressure Service events which are passed to
 *          the application.
 *
 * @param[in]   p_bps   Blood Pressure Service structure.
 * @param[in]   p_evt   Event received from the Blood Pressure Service.
 */
void on_bps_evt(ble_bps_t * p_bps, ble_bps_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_BPS_EVT_INDICATION_ENABLED:
            // Indication has been enabled, send a single blood pressure measurement.
           // blood_pressure_measurement_send();
            break;

        case BLE_BPS_EVT_INDICATION_CONFIRMED:
            m_bps_meas_ind_conf_pending = false;
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Blood Pressure, Battery, and Device Information services.
 */
void services_init(void)
{
    uint32_t         err_code;
    ble_bps_init_t   bps_init;
    ble_bas_init_t   bas_init;
    ble_dis_init_t   dis_init;
    ble_dis_sys_id_t sys_id;

    // Initialize Blood Pressure Service.
    memset(&bps_init, 0, sizeof(bps_init));

    bps_init.evt_handler = on_bps_evt;
    bps_init.feature     = BLE_BPS_FEATURE_BODY_MOVEMENT_BIT |
                           BLE_BPS_FEATURE_MEASUREMENT_POSITION_BIT;

    // Here the sec level for the Blood Pressure Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&bps_init.bps_meas_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bps_init.bps_meas_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bps_init.bps_meas_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bps_init.bps_feature_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bps_init.bps_feature_attr_md.write_perm);

    err_code = ble_bps_init(&m_bps, &bps_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Battery Service.
    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.model_num_str,     MODEL_NUM);

    sys_id.manufacturer_id            = MANUFACTURER_ID;
    sys_id.organizationally_unique_id = ORG_UNIQUE_ID;
    dis_init.p_sys_id                 = &sys_id;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting application timers.
 */
void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Connection Parameter events.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail configuration parameter, but instead we use the
 *                event handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t connection_params_init;

    memset(&connection_params_init, 0, sizeof(connection_params_init));

    connection_params_init.p_conn_params                  = NULL;
    connection_params_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    connection_params_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    connection_params_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    connection_params_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    connection_params_init.disconnect_on_fail             = false;
    connection_params_init.evt_handler                    = on_conn_params_evt;
    connection_params_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&connection_params_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();

            break;
        default:
            break;
    }
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);

            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle               = BLE_CONN_HANDLE_INVALID;
            m_bps_meas_ind_conf_pending = false;
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            if (p_ble_evt->evt.gatts_evt.params.timeout.src == BLE_GATT_TIMEOUT_SRC_PROTOCOL)
            {
                err_code = sd_ble_gap_disconnect(m_conn_handle,
                                                 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_bps_on_ble_evt(&m_bps, p_ble_evt);
    ble_bas_on_ble_evt(&m_bas, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    dm_ble_evt_handler(p_ble_evt);
    bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in] sys_evt  System stack event.
 */
void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);
    
    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    
    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in] event  Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            err_code = ble_advertising_restart_without_whitelist();
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_KEY_0:
            if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
            {
               // blood_pressure_measurement_send();
            }
            break;

        default:
            break;
    }
}


/**@brief Function for handling the Device Manager events.
 *
 * @param[in]   p_evt   Data associated to the device manager event.
 */
uint32_t device_manager_evt_handler(dm_handle_t const * p_handle,
                                           dm_event_t const  * p_event,
                                           ret_code_t        event_result)
{
    uint32_t err_code;
    bool     is_indication_enabled;

    APP_ERROR_CHECK(event_result);

    switch(p_event->event_id)
    {
        case DM_EVT_LINK_SECURED:
            // Send a single blood pressure measurement if indication is enabled.
            // NOTE: For this to work, make sure ble_bps_on_ble_evt() is called before
            //       ble_bondmngr_on_ble_evt() in ble_evt_dispatch().
            err_code = ble_bps_is_indication_enabled(&m_bps, &is_indication_enabled);
            APP_ERROR_CHECK(err_code);

            if (is_indication_enabled)
            {
               // blood_pressure_measurement_send();
            }
            break;

        default:
            // No implementation needed.
            break;
    }

    return NRF_SUCCESS;
}


/**@brief Function for the Device Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Device Manager.
 */
void device_manager_init(bool erase_bonds)
{
    uint32_t               err_code;
    dm_init_param_t        init_param = {.clear_persistent_data = erase_bonds};
    dm_application_param_t register_param;

    // Initialize persistent storage module.
    err_code = pstorage_init();
    APP_ERROR_CHECK(err_code);

    err_code = dm_init(&init_param);
    APP_ERROR_CHECK(err_code);

    memset(&register_param.sec_param, 0, sizeof(ble_gap_sec_params_t));

    register_param.sec_param.bond         = SEC_PARAM_BOND;
    register_param.sec_param.mitm         = SEC_PARAM_MITM;
    register_param.sec_param.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    register_param.sec_param.oob          = SEC_PARAM_OOB;
    register_param.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    register_param.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    register_param.evt_handler            = device_manager_evt_handler;
    register_param.service_type           = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

    err_code = dm_register(&m_app_handle, &register_param);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                                 bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for the Power manager.
 */
void BLE_power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

void Ble_Init(void){		// Initialize.
	  bool erase_bonds;   
    timers_init();
    buttons_leds_init(&erase_bonds);
    ble_stack_init();
    device_manager_init(erase_bonds);
    gap_params_init();
    advertising_init();
    services_init();
    //sensor_simulator_init();
    conn_params_init();
    // Start execution.
    application_timers_start();
    APP_ERROR_CHECK(ble_advertising_start(BLE_ADV_MODE_FAST));	
}
