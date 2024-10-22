# Camera API for micropython

[![ESP32 Port](https://github.com/cnadler86/micropython-camera-API/actions/workflows/ESP32.yml/badge.svg)](https://github.com/cnadler86/micropython-camera-API/actions/workflows/ESP32.yml)

This project aims to support cameras in different ports in micropython, starting with the ESP32-Port and omnivision (OV2640 & OV5640) cameras. The project implements a general API for cameras in micropython (such as circuitpython have done).
At the moment, this is a micropython user module, but it might get in the micropython repo in the future.
The API is stable, but it might change without previous announce.

## Precomiled FW (the easy way)

If you are not familiar with building a custom firmware, you can go to the [releases](https://github.com/cnadler86/micropython-camera-API/releases) page and download the firmwares that suits your board.

## Using the API

```python
from camera import Camera, GrabMode, PixelFormat, FrameSize, GainCeiling

#Camera construction using defaults (if you are using a non-generic precompiled firmware or if you specified them in mpconfigboard.h during your build)
camera = Camera()

# Camera construction needs some keyword arguments, if using a generic precompiled firmware
# These pins are just examples and if you use them just like that will get a watchdog error. Adapt them to your board!
camera = Camera(
    data_pins=[1,2,3,4,5,6,7,8],
    vsync_pin=9,
    href_pin=10,
    sda_pin=11,
    scl_pin=12,
    pclk_pin=13,
    xclk_pin=14,
    xclk_freq=20000000,
    powerdown_pin=-1,
    reset_pin=-1,
    pixel_format=PixelFormat.RGB565,
    frame_size=FrameSize.QVGA,
    jpeg_quality=85,
    fb_count=1,
    grab_mode=GrabMode.WHEN_EMPTY
)

# Capture image
img = camera.capture()

# Camera reconfiguration 
camera.reconfigure(pixel_format=PixelFormat.JPEG,frame_size=FrameSize.QVGA,grab_mode=GrabMode.LATEST, fb_count=2)
camera.set_quality(90)  # The quality goes from 0% to 100%, meaning 100% is the highest but has probably no compression
```

You can get and set sensor properties by the respective methods (e.g. camera.get_brightness() or camera.set_vflip(True). See autocompletions in Thonny in order to see the list of methods.
If you want more insides in the methods and what they actually do, you can find a very good documentation [here](https://docs.circuitpython.org/en/latest/shared-bindings/espcamera/index.html).
Notice that for the methods in here you need to prefix a get/set, depending on what you want to do.

## Default values

The default values of the following keyword arguments are:

- xclk_freq: 20MHz    // Frequencies are normally either 10 MHz or 20 MHz
- frame_size: QQVGA
- pixel_format: RGB565
- jpeg_quality: 85    // Quality of JPEG output in percent. Higher means higher quality.
- powerdown_pin and reset_pin: -1 (not used/available/needed)
- fb_count:
  - 2 for ESP32S3 boards
  - 1 for all other
- grab_mode:
  - LATEST for ESP32S3 boards
  - WHEN_EMPTY for all other

## Build your custom FW

### Setup build environment (the DIY way)

To build the project, follow the following instructions:

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.2.3/esp32/get-started/index.html): I used version 5.2.3, but it might work with other versions (see notes).
- Clone the micropython repo and this repo in a folder, e.g. "MyESPCam". Micropython >=1.24 is required (al least >= commit 92484d8).
- You will have to add the ESP32-Camera driver (I used v2.0.13). To do this, add the following to the respective idf_component.yml file  (e.g. in micropython/ports/esp32/main_esp32s3/idf_component.yml):

```yml
  espressif/esp32-camera:
    git: https://github.com/espressif/esp32-camera
```

Alternatively, you can clone the <https://github.com/espressif/esp32-camera> repository inside the esp-idf/components folder instead of altering the idf_component.yml file.

### Add camera configurations to your board (Optional, but recommended)

#### Supported Camera Models

This project supports various camera models out of the box. So, you basicalls only need to add only one line to your board config-file ("mpconfigboard.h).
Example (don't forget to add the empty line at the bottom):

```c
#define MICROPY_CAMERA_MODEL_WROVER_KIT       1

```

Below is a list of the possible definitions for `MICROPY_CAMERA_MODEL_xxx` and links to the corresponding boards:

- **MICROPY_CAMERA_MODEL_WROVER_KIT**
  - [ESP32-WROVER-KIT](https://www.espressif.com/en/products/devkits/esp32-wrover-kit/overview)

- **MICROPY_CAMERA_MODEL_ESP_EYE**
  - [ESP-EYE](https://www.espressif.com/en/products/devkits/esp-eye/overview)

- **MICROPY_CAMERA_MODEL_M5STACK_PSRAM**
  - [M5Stack PSRAM](https://shop.m5stack.com/collections/m5-cameras)

- **MICROPY_CAMERA_MODEL_M5STACK_UNITCAM**
  - [M5Stack UnitCam](https://shop.m5stack.com/collections/m5-cameras)

- **MICROPY_CAMERA_MODEL_M5STACK_V2_PSRAM**
  - [M5Stack V2 PSRAM](https://shop.m5stack.com/collections/m5-cameras)

- **MICROPY_CAMERA_MODEL_M5STACK_WIDE**
  - [M5Stack Wide](https://shop.m5stack.com/collections/m5-cameras)

- **MICROPY_CAMERA_MODEL_M5STACK_ESP32CAM**
  - [M5Stack ESP32CAM](https://shop.m5stack.com/collections/m5-cameras)

- **MICROPY_CAMERA_MODEL_M5STACK_CAMS3_UNIT**
  - [M5Stack CAMS3 Unit](https://shop.m5stack.com/collections/m5-cameras)

- **MICROPY_CAMERA_MODEL_AI_THINKER**
  - [AI-Thinker ESP32-CAM](https://www.ai-thinker.com/product/esp32-cam)

- **MICROPY_CAMERA_MODEL_XIAO_ESP32S3**
  - [XIAO ESP32S3](https://www.seeedstudio.com/xiao-series-page)

- **MICROPY_CAMERA_MODEL_ESP32_MP_CAMERA_BOARD**
  - [ESP32 MP Camera Board]

- **MICROPY_CAMERA_MODEL_ESP32S3_CAM_LCD**
  - [ESP32-S3 CAM LCD]

- **MICROPY_CAMERA_MODEL_ESP32S3_EYE**
  - [ESP32-S3 EYE](https://www.espressif.com/en/products/devkits/esp32-s3-eye/overview)

- **MICROPY_CAMERA_MODEL_FREENOVE_ESP32S3_CAM**
  - [Freenove ESP32-S3 CAM](https://store.freenove.com/products/fnk0085)

- **MICROPY_CAMERA_MODEL_DFRobot_ESP32S3**
  - [DFRobot ESP32-S3](https://www.dfrobot.com/)

- **MICROPY_CAMERA_MODEL_TTGO_T_JOURNAL**
  - [TTGO T-Journal](https://www.lilygo.cc/products/)

- **MICROPY_CAMERA_MODEL_TTGO_T_CAMERA_PLUS**
  - [TTGO T-Camera Plus](https://www.lilygo.cc/products/)

- **MICROPY_CAMERA_MODEL_NEW_ESPS3_RE1_0**
  - [New ESP32-S3 RE:1.0]

- **MICROPY_CAMERA_MODEL_XENOIONEX**
  - [Xenoionex]

#### For unsupported camera models

If the board is not supported yet, add the following lines to your board config-file "mpconfigboard.h" with the respective pins and camera parameters. Otherwise, you will need to pass all parameters during construction.
Don't forget the empty line at the bottom.
Example for Xiao sense:

```c
#define MICROPY_CAMERA_PIN_D0       (15)
#define MICROPY_CAMERA_PIN_D1       (17)
#define MICROPY_CAMERA_PIN_D2       (18)
#define MICROPY_CAMERA_PIN_D3       (16)
#define MICROPY_CAMERA_PIN_D4       (14)
#define MICROPY_CAMERA_PIN_D5       (12)
#define MICROPY_CAMERA_PIN_D6       (11)
#define MICROPY_CAMERA_PIN_D7       (48)
#define MICROPY_CAMERA_PIN_PCLK     (13)
#define MICROPY_CAMERA_PIN_VSYNC    (38)
#define MICROPY_CAMERA_PIN_HREF     (47)
#define MICROPY_CAMERA_PIN_XCLK     (10)
#define MICROPY_CAMERA_PIN_PWDN     (-1)
#define MICROPY_CAMERA_PIN_RESET    (-1)
#define MICROPY_CAMERA_PIN_SIOD     (40)  // SDA
#define MICROPY_CAMERA_PIN_SIOC     (39)  // SCL
#define MICROPY_CAMERA_XCLK_FREQ    (20000000)  // Frequencies are normally either 10 MHz or 20 MHz
#define MICROPY_CAMERA_FB_COUNT     (2)   // The value is between 1 (slow) and 2 (fast, but more load on CPU and more ram usage)
#define MICROPY_CAMERA_JPEG_QUALITY (85)  // Quality of JPEG output in percent. Higher means higher quality.
#define MICROPY_CAMERA_GRAB_MODE    (1)   // 0=WHEN_EMPTY (might have old data, but less resources), 1=LATEST (best, but more resources)

```

### Build the API

To build the project, you could do it the following way:

```bash
. <path2esp-idf>/esp-idf/export.sh
cd MyESPCam/micropython/ports/esp32
make USER_C_MODULES=../../../../micropython-camera-API/src/micropython.cmake BOARD=<Your-Board> clean
make USER_C_MODULES=../../../../micropython-camera-API/src/micropython.cmake BOARD=<Your-Board> submodules
make USER_C_MODULES=../../../../micropython-camera-API/src/micropython.cmake BOARD=<Your-Board> all
```

If you experience problems, visit [MicroPython external C modules](https://docs.micropython.org/en/latest/develop/cmodules.html).

## Notes

- For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.
- The OV5640 pinout is compatible with boards designed for the OV2640 but the voltage supply is too high for the internal 1.5V regulator, so the camera overheats unless a heat sink is applied. For recording purposes the OV5640 should only be used with an ESP32S3 board. Frame sizes above FHD framesize should only be used for still images due to memory limitations.
- If your target board is a ESP32, I recommend using IDF >= 5.2, since older versions may lead to IRAM overflow during build. Alternatively you can modify your sdkconfig-file (see [issue #1](https://github.com/cnadler86/micropython-camera-API/issues/1)).

## Plans for the future

- [ ] edge case: enable usage of pins such as i2c for other applications
- [ ] provide examples in binary image with lfs-merge
- [ ] include camera driver version in API
