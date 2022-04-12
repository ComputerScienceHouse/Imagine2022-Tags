#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#include "config.h"
#include "bluetooth.h"

const uint8_t custom_mac[] = {
    0xBE, 0xEF, 0x34, 0x25, 0x69, CONFIG_TAG_NUM
};

void app_main(void)
{
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(CSHA_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_BLE)) != ESP_OK) 
    {
        ESP_LOGE(CSHA_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_init()) != ESP_OK) 
    {
        ESP_LOGE(CSHA_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_bluedroid_enable()) != ESP_OK) 
    {
        ESP_LOGE(CSHA_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

    set_uuid(config_device_uuid);
    if ((ret = esp_base_mac_addr_set(custom_mac))  != ESP_OK)
    {
        ESP_LOGE(CSHA_TAG, "%s Could not set mac address %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else
    {
        uint8_t bt_addr[6];
        esp_read_mac(bt_addr, ESP_MAC_BT);
        ESP_LOGI(CSHA_TAG, "bt address set: %02x:%02x:%02x:%02x:%02x:%02x", 
            bt_addr[0],
            bt_addr[1],
            bt_addr[2],
            bt_addr[3],
            bt_addr[4],
            bt_addr[5]
        );
    }

    size_t dev_count = sizeof(whitelisted_bdas) / sizeof(whitelisted_bdas[0]);

    start_ble_beacon(whitelisted_bdas, ENABLE_DEVICE_WHITELIST ? dev_count: 0);

    ESP_LOGI("LIGMA", "Done!");
}

