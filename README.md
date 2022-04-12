# CSHacked—Embedded Tags

Embedded code running on each of the tracking tags for ImagineRIT 2022

## How to use

Set a unique device number with idf.py menuconfig and navigate to section labeled Main



### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use. For example, /dev/ttyUSB0)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

- Some old BT devices don't place their device name in EIR. Users can obtain the peer device name in `bt_app_gap_cb` `ESP_BT_GAP_DISC_RES_EVT` event handler or resolve it in EIR as in the function `get_name_from_eir`.

- The ESP32 places its device name in EIR by default.
