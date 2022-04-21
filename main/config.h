/*
* copy to config.h and add secrets
*/
#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "esp_bt_defs.h"

#define DEVICE_MANUFACTURER "Prof D Glizzy"

#define ENABLE_DEVICE_WHITELIST false

#define CONFIG_TAG_NUM 0x05

// must be unicast address
// first byte LSB is 0
// first byte is even
// !!! BT derived addr will increase last byte by 2 !!!!

static const char* BT_DEVICE_NAME = "Smell1";


static uint8_t whitelisted_bdas[][6] = {
    {0xc0, 0x6b, 0x55, 0x93, 0x0c, 0xab},
    {0x00, 0xee, 0x00, 0x02, 0xd0, 0xc3},
    {0xf6, 0xf7, 0xf7, 0xf7, 0xf7, 0x00}
    };

// unique UUIDs required
static uint8_t config_device_uuid[ESP_UUID_LEN_128] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,
    //second uuid, 32bit, [12], [13], [14], [15] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

#endif
