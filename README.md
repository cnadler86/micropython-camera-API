# Camera API for micropython

[![ESP32 Port](https://github.com/cnadler86/micropython-camera-API/actions/workflows/ESP32.yml/badge.svg)](https://github.com/cnadler86/micropython-camera-API/actions/workflows/ESP32.yml)

This project aims to support various cameras (e.g. OV2640, OV5640) on different MicroPython ports, starting with the ESP32 port. The project implements a general API, has precompiled FW images and supports a lot of cameras out of the box.
At the moment, this is a micropython user module, but it might get in the micropython repo in the future.
The API is stable, but it might change without previous announce.

## Precomiled FW (the easy way)

If you are not familiar with building custom firmware, visit the [releases](https://github.com/cnadler86/micropython-camera-API/releases) page to download firmware that suits your board. **There are over 20 precompiled board images with the latest micropython!**

## Using the API

### Importing the Camera Module

```python
from camera import Camera, GrabMode, PixelFormat, FrameSize, GainCeiling
```

### Creating a Camera Object

Camera construction using defaults. This is the case if you are using a **non-generic** precompiled firmware or if you specified the camera model or pins in mpconfigboard.h during your build. Then you can just call the construction without any keyword arguments.

```python
cam = Camera()
```

or with relevant keyword arguments:

```python
cam = Camera(pixel_format=PixelFormat.JPEG,
    frame_size=FrameSize.QVGA,
    jpeg_quality=90,
    fb_count=2,
    grab_mode=GrabMode.WHEN_EMPTY)
```

When using a **generic** precompiled firmware, the camera constructor requires specific keyword arguments (namely the camera pins to be used).
These pins are just examples and if used as-is, a error will occur. Adapt them to your board!

```python
cam = Camera(
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
)
```

**Keyword arguments for construction:**

- data_pins: List of data pins
- pclk_pin: Pixel clock pin
 -vsync_pin: VSYNC pin
- href_pin: HREF pin
- sda_pin: SDA pin
- scl_pin: SCL pin
- xclk_pin: XCLK pin
- xclk_freq: XCLK frequency in Hz
- powerdown_pin: Powerdown pin (default: -1, meaning not used)
- reset_pin: Reset pin (default: -1, meaning not used)
- pixel_format: Pixel format as PixelFormat
- frame_size: Frame size as FrameSize
- jpeg_quality: JPEG quality
- fb_count: Frame buffer count
- grab_mode: Grab mode as GrabMode
- init: Initialize camera at construction time (default: True)
- bmp_out: Image capture output converted to bitmap (default: False)

**Default values:**

The following keyword arguments have default values:

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

### Initializing the Camera

```python
cam.init()
```

### Capture image

```python
img = cam.capture()
```

Keyword arguments for capture

- out_format: Output format as PixelFormat (optional)

### Camera reconfiguration

```python
cam.reconfigure(pixel_format=PixelFormat.JPEG,frame_size=FrameSize.QVGA,grab_mode=GrabMode.LATEST, fb_count=2)
```

Keyword arguments for reconfigure

- frame_size: Frame size as FrameSize (optional)
- pixel_format: Pixel format as PixelFormat(optional)
- grab_mode: Grab mode as GrabMode (optional)
- fb_count: Frame buffer count (optional)

### Additional methods

Here are just a few examples:

```python
cam.set_quality(90)  # The quality goes from 0% to 100%, meaning 100% is the highest but has probably no compression
cam.set_bmp_out(True) # Enables convertion to bmp when capturing image
camera.get_brightness()
camera.set_vflip(True) #Enable vertical flip
```

See autocompletions in Thonny in order to see the list of methods.
If you want more insides in the methods and what they actually do, you can find a very good documentation [here](https://docs.circuitpython.org/en/latest/shared-bindings/espcamera/index.html).
Note that each method requires a "get_" or "set_" prefix, depending on the desired action.

To get the version of the camera driver used:

```python
import camera
vers = camera.Version()
```

### Additional information

The FW images support the following cameras out of the box, but is therefore big: OV7670, OV7725, OV2640, OV3660, OV5640, NT99141, GC2145, GC032A, GC0308, BF3005, BF20A6, SC030IOT

## Build your custom FW

### Setting up the build environment (DIY method)

To build the project, follow these instructions:

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.2.3/esp32/get-started/index.html): I used version 5.2.3, but it might work with other versions (see notes).
- Clone the micropython repo and this repo in a folder, e.g. "MyESPCam". MicroPython version 1.24 or higher is required (at least commit 92484d8).
- You will have to add the ESP32-Camera driver (I used v2.0.15). To do this, add the following to the respective idf_component.yml file (e.g. in micropython/ports/esp32/main_esp32s3/idf_component.yml):

```yml
  espressif/esp32-camera:
    git: https://github.com/espressif/esp32-camera.git
```

Alternatively, you can clone the <https://github.com/espressif/esp32-camera> repository inside the esp-idf/components folder instead of altering the idf_component.yml file.

### Add camera configurations to your board (optional, but recommended)

#### Supported Camera Models

This project supports various boards with camera interface out of the box. You typically only need to add a single line to your board config file ("mpconfigboard.h).
Example (don't forget to add the empty line at the bottom):

```c
#define MICROPY_CAMERA_MODEL_WROVER_KIT       1

```

Below is a list of supported `MICROPY_CAMERA_MODEL_xxx` definitions:

- MICROPY_CAMERA_MODEL_WROVER_KIT - [ESP32-WROVER-KIT](https://www.espressif.com/en/products/devkits/esp32-wrover-kit/overview)
- MICROPY_CAMERA_MODEL_ESP_EYE - [ESP-EYE](https://www.espressif.com/en/products/devkits/esp-eye/overview)
- MICROPY_CAMERA_MODEL_M5STACK_PSRAM - [M5Stack PSRAM](https://shop.m5stack.com/collections/m5-cameras)
- MICROPY_CAMERA_MODEL_M5STACK_UNITCAM - [M5Stack UnitCam](https://shop.m5stack.com/collections/m5-cameras)
- MICROPY_CAMERA_MODEL_M5STACK_V2_PSRAM - [M5Stack V2 PSRAM](https://shop.m5stack.com/collections/m5-cameras)
- MICROPY_CAMERA_MODEL_M5STACK_WIDE - [M5Stack Wide](https://shop.m5stack.com/collections/m5-cameras)
- MICROPY_CAMERA_MODEL_M5STACK_ESP32CAM - [M5Stack ESP32CAM](https://shop.m5stack.com/collections/m5-cameras)
- MICROPY_CAMERA_MODEL_M5STACK_CAMS3_UNIT - [M5Stack CAMS3 Unit](https://shop.m5stack.com/collections/m5-cameras)
- MICROPY_CAMERA_MODEL_AI_THINKER - [AI-Thinker ESP32-CAM]
- MICROPY_CAMERA_MODEL_XIAO_ESP32S3 - [XIAO ESP32S3](https://www.seeedstudio.com/xiao-series-page)
- MICROPY_CAMERA_MODEL_ESP32_MP_CAMERA_BOARD - [ESP32 MP Camera Board]
- MICROPY_CAMERA_MODEL_ESP32S3_CAM_LCD - [ESP32-S3 CAM LCD]
- MICROPY_CAMERA_MODEL_ESP32S3_EYE - [ESP32-S3 EYE](https://www.espressif.com/en/products/devkits/esp32-s3-eye/overview)
- MICROPY_CAMERA_MODEL_FREENOVE_ESP32S3_CAM - [Freenove ESP32-S3 CAM](https://store.freenove.com/products/fnk0085)
- MICROPY_CAMERA_MODEL_DFRobot_ESP32S3 - [DFRobot ESP32-S3](https://www.dfrobot.com/)
- MICROPY_CAMERA_MODEL_TTGO_T_JOURNAL - [TTGO T-Journal](https://www.lilygo.cc/products/)
- MICROPY_CAMERA_MODEL_TTGO_T_CAMERA_PLUS - [TTGO T-Camera Plus](https://www.lilygo.cc/products/)
- MICROPY_CAMERA_MODEL_NEW_ESPS3_RE1_0 - [New ESP32-S3 RE:1.0]
- MICROPY_CAMERA_MODEL_XENOIONEX - [Xenoionex]

#### For unsupported camera models

If your board is not yet supported, add the following lines to your board config-file "mpconfigboard.h" with the respective pins and camera parameters. Otherwise, you will need to pass all parameters during construction.
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
#### Customize additional camera settings

If you want to customize additional camera setting or reduce the FW size by removing support for unused camera sensors, then take a look at the kconfig file of the esp32-camera driver and specify these on the sdkconfig file of your board.

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

- For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has significantly improved, but JPEG mode always gives better frame rates.
- The OV5640 pinout is compatible with boards designed for the OV2640 but the voltage supply is too high for the internal 1.5V regulator, so the camera overheats unless a heat sink is applied. For recording purposes the OV5640 should only be used with an ESP32S3 board. Frame sizes above FHD framesize should only be used for still images due to memory limitations.
- If your target board is a ESP32, I recommend using IDF >= 5.2, since older versions may lead to IRAM overflow during build. Alternatively you can modify your sdkconfig-file (see [issue #1](https://github.com/cnadler86/micropython-camera-API/issues/1)).
- The driver requires PSRAM to be installed and activated.
- Most of the precompiled firmware images are untested, but the only difference between them are the target architecture and pin definitions, so they should work out of the box. If not, please raise an issue.

## FPS benchmark

I didn't use a calibrated osziloscope, but here is a benchmark with my ESP32S3 (GrabMode=LATEST, fb_count = 1, jpeg_quality=85%).
Using fb_count=2 theoretically can double the FPS (see JPEG with fb_count=2). This might also aplly for other PixelFormats.

| Frame Size | GRAYSCALE | RGB565 | YUV422 | JPEG   | JPEG -> RGB565 | JPEG -> RGB888 | JPEG (fb=2) |
|------------|-----------|--------|--------|--------|----------------|----------------|-------------|
| R96X96     | 12.5      | 12.5   | 12.5   | No img | No img         | No img         | No img      |
| QQVGA      | 12.5      | 12.5   | 12.5   | 25     | 25             | 25             | 50          |
| QCIF       | 11        | 11     | 11.5   | 25     | 25             | 25             | 50          |
| HQVGA      | 12.5      | 12.5   | 12.5   | 25     | 16.7           | 16.7           | 50          |
| R240X240   | 12.5      | 12.5   | 11.5   | 25     | 16.7           | 12.5           | 50          |
| QVGA       | 12        | 11     | 12     | 25     | 12.5           | 12.5           | 50          |
| CIF        | 12.5      | No img | No img | 6.3    | 1.6            | 1.6            | 12.5        |
| HVGA       | 3         | 3      | 2.5    | 12.5   | 6.3            | 6.3            | 25          |
| VGA        | 3         | 3      | 3      | 12.5   | 3.6            | 3.6            | 25          |
| SVGA       | 3         | 3      | 3      | 12.5   | 2.8            | 2.5            | 25          |
| XGA        | No img    | No img | No img | 6.3    | 1.6            | 1.6            | 12.5        |
| HD         | No img    | No img | No img | 6.3    | 1.4            | 1.3            | 12.5        |
| SXGA       | 2         | 2      | 2      | 6.3    | 1              | 1              | 12.5        |
| UXGA       | No img    | No img | No img | 6.3    | 0.7            | 0.7            | 12.5        |


Looking at the results: image conversion make only sense for frame sized below QVGA or if capturing the image in the intended pixelformat and frame size combination fails.

## Troubleshoot

You can find information on the following sites:
- [ESP-FAQ](https://docs.espressif.com/projects/esp-faq/en/latest/application-solution/camera-application.html)
- [ChatGPT](https://chatgpt.com/)
- [Issues in here](https://github.com/cnadler86/micropython-camera-API/issues?q=is%3Aissue)

## Future Plans

- Edge case: enable usage of pins such as i2c for other applications
- Provide examples in binary image
