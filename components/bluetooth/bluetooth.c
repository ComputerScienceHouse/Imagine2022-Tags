#include "bluetooth.h"

static uint8_t identifier_mac[6];
static esp_ble_adv_params_t advert_params;
static uint8_t gatt_uuid[ESP_UUID_LEN_128];
static uint8_t tag_manufacturer[] = {0x12,0x34,0x56,0x78};
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 4, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  &tag_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(gatt_uuid),
    .p_service_uuid = gatt_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

char *bda2str(esp_bd_addr_t bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18) 
    {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
            p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}

char *uuid2str(esp_bt_uuid_t *uuid, char *str, size_t size)
{
    if (uuid == NULL || str == NULL) 
    {
        return NULL;
    }
    if (uuid->len == 2 && size >= 5) 
    {
        sprintf(str, "%04x", uuid->uuid.uuid16);
    }
    else if (uuid->len == 4 && size >= 9) 
    {
        sprintf(str, "%08x", uuid->uuid.uuid32);
    }
    else if (uuid->len == 16 && size >= 37)
    {
        uint8_t *p = uuid->uuid.uuid128;
        sprintf(str, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                p[15], p[14], p[13], p[12], p[11], p[10], p[9], p[8],
                p[7], p[6], p[5], p[4], p[3], p[2], p[1], p[0]);
    } 
    else 
    {
        return NULL;
    }

    return str;
}
void esp_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;
    switch (event) {
    //case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    //case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        if ((err = set_whitelist(wl_devs, wl_dev_count)) != ESP_OK) 
        {
            ESP_LOGE(CSHA_TAG, "Could not set whitelist");
            return;
        }
        ESP_LOGI(CSHA_TAG, "Whitelist set");
        break;
    case ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT:
        wls_set++;
        if (wls_set == wl_dev_count) esp_ble_gap_start_advertising(&advert_params);
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        ESP_LOGI(CSHA_TAG, "Advertise started");
        break;
    case ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT:
        scan_info = (ble_scan_req*) param;
        char bda_str[18];

        bda2str(scan_info->scan_addr, bda_str, 18);
        ESP_LOGI(CSHA_TAG, "Scan from %s", bda_str);
        break;
    default:
        ESP_LOGI(CSHA_TAG, "%d", event);
        break;
    }
}
esp_ble_adv_params_t* set_tag_adv_params(esp_ble_adv_params_t* param_struct)
{
    memset(param_struct, 0, sizeof(*param_struct));
    
    param_struct->adv_int_min = 0x0800;
    param_struct->adv_int_max = 0x0800;

    //param_struct->adv_type = ADV_TYPE_DIRECT_IND_HIGH;
    //param_struct->adv_type = ADV_TYPE_DIRECT_IND_LOW;
    param_struct->adv_type = ADV_TYPE_SCAN_IND;
    //param_struct->adv_type = ADV_TYPE_IND;

    // owner device address type
    param_struct->own_addr_type = BLE_ADDR_TYPE_PUBLIC;
    //param_struct->own_addr_type = BLE_ADDR_TYPE_RANDOM;
    //param_struct->own_addr_type = BLE_ADDR_TYPE_RPA_PUBLIC;
    //param_struct->own_addr_type = BLE_ADDR_TYPE_RPA_RANDOM;

    // Peer device BDA
    //param_struct->peer_addr = some_address (uint8_t[6])

    // Peer device bluetooth device address type
    // only supports public address type and random address type
    //param_struct->peer_addr_type = BLE_ADDR_TYPE_PUBLIC;
    //param_struct->peer_addr_type = BLE_ADDR_TYPE_RANDOM;

    // Advertising channel map
    // param_struct->channel_map = ADV_CHNL_37;
    // param_struct->channel_map = ADV_CHNL_38;
    // param_struct->channel_map = ADV_CHNL_39;
    param_struct->channel_map = ADV_CHNL_ALL;

    // Advertising filter (THE IMPORTANT BIT!!!)
    param_struct->adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY; // Allow both scan and connection requests from anyone.
    //param_struct->adv_filter_policy = ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY; // Allow both scan req from White List devices only and connection req from anyone.
    //param_struct->adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST; // Allow both scan req from anyone and connection req from White List devices only.
    //param_struct->adv_filter_policy = ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST; // Allow scan and connection requests from White List devices only.


    return param_struct;
}
esp_err_t set_whitelist(uint8_t whitelisted_devices[][6], size_t device_count)
{
    esp_err_t err = esp_ble_gap_clear_whitelist();
    if (err != ESP_OK) return err;
    wls_set = 0;
    for (int i = 0; i < device_count; i++)
    {
        if ((err = esp_ble_gap_update_whitelist(true, whitelisted_devices[i], BLE_WL_ADDR_TYPE_PUBLIC)) != ESP_OK) return err;
    }

    return ESP_OK;
}
void set_uuid(uint8_t uuid[ESP_UUID_LEN_128])
{
    memcpy(gatt_uuid, uuid, ESP_UUID_LEN_128 * sizeof(uint8_t));
}
void start_ble_beacon(uint8_t whitelisted_devices[][6], size_t device_count)
{
    esp_efuse_mac_get_default(identifier_mac);
    char bda_str[18];
    bda2str(identifier_mac, bda_str, 18);

    ESP_LOGI(CSHA_TAG,"Controller BDA: %s", bda_str);
    set_tag_adv_params(&advert_params);

    esp_err_t err;
    
    wl_devs = whitelisted_devices;
    wl_dev_count = device_count;

    ESP_LOGI(CSHA_TAG, "print devices");
    for (int i = 0; i < device_count; i++)
    {
        ESP_LOGI(CSHA_TAG, "whitelisted device: %02x:%02x:%02x:%02x:%02x:%02x",
            whitelisted_devices[i][0], 
            whitelisted_devices[i][1], 
            whitelisted_devices[i][2], 
            whitelisted_devices[i][3], 
            whitelisted_devices[i][4], 
            whitelisted_devices[i][5]
        );
    }
    esp_ble_gap_register_callback(esp_gap_callback);
    esp_ble_gap_config_adv_data(&adv_data);
    //err = esp_ble_gap_start_advertising(&advert_params);
    //ESP_LOGI(CSHA_TAG, "Advertise started");
    for(;;)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}
