#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "nvs.h"
//#include "nvs_flash.h"
//#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"

#define CSHA_TAG "CSHA BLE"

/*
* possible BLE app states or states 
*/
typedef enum {
    APP_GAP_STATE_IDLE = 0,
    APP_GAP_STATE_DEVICE_DISCOVERING,
    APP_GAP_STATE_DEVICE_DISCOVER_COMPLETE,
    APP_GAP_STATE_SERVICE_DISCOVERING,
    APP_GAP_STATE_SERVICE_DISCOVER_COMPLETE,
} app_gap_state_t;

/*
* format Bluetooth Device Address into string
* returns pointer to string
*/
char *bda2str(esp_bd_addr_t bda, char *str, size_t size);

/*
* format UUID to string
*/
char *uuid2str(esp_bt_uuid_t *uuid, char *str, size_t size);
/*
* gap advertise callback
*/
void esp_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/*
* set advertising parameters to ensure correct mode and discoverers
*/
esp_ble_adv_params_t* set_tag_adv_params(esp_ble_adv_params_t* param_struct); 

/*
* start ble advertise
*/
void start_ble_beacon(uint8_t whitelisted_devices[][6], size_t device_count);

/*
* set GATT uuid of device for response data
*/
void set_uuid(uint8_t uuid[ESP_UUID_LEN_128]);

/*
* pass a list of BDAs to add to broadcast whitelist
*/
esp_err_t set_whitelist(uint8_t whitelisted_devices[][6], size_t device_count);

#endif

