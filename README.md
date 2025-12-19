# Camera API for micropython

[![ESP32 Port](https://github.com/cnadler86/micropython-camera-API/actions/workflows/ESP32.yml/badge.svg)](https://github.com/cnadler86/micropython-camera-API/actions/workflows/ESP32.yml)

This project aims to support various cameras (e.g. OV2640, OV5640) on different MicroPython ports, starting with the ESP32 port. The project implements a general API, has precompiled firmware images and supports a lot of cameras out of the box. Defaults are set to work with the OV2640.

The API is stable, but it might change. Please look into the release section for the latest changes.

I tied to make things as easy as possible. If you find this project useful, please consider donating to support my work. Thanks!
[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.me/cnadler)

If you want to play arround with AI, take a look at the [micropython binding for esp-dl](https://github.com/cnadler86/mp_esp_dl_models).

## Content

- [Precompiled firmware (the easy way)](#precompiled-firmware-the-easy-way)
- [Using the API](#using-the-api)
  - [Importing the camera module](#importing-the-camera-module)
  - [Creating a camera object](#creating-a-camera-object)
  - [Initializing the camera](#initializing-the-camera)
  - [Capture image](#capture-image)
  - [Camera reconfiguration](#camera-reconfiguration)
  - [Freeing the buffer](#freeing-the-buffer)
  - [Is a frame available](#is-frame-available)
  - [Additional methods and examples](#additional-methods-and-examples)
  - [I2C Integration](#i2c-integration)
  - [Additional information](#additional-information)
- [Build your custom firmware](#build-your-custom-firmware)
  - [Setting up the build environment (DIY method)](#setting-up-the-build-environment-diy-method)
  - [Add camera configurations to your board (optional, but recommended)](#add-camera-configurations-to-your-board-optional-but-recommended)
  - [Build the API](#build-the-api)
- [Notes](#notes)
- [Benchmark](#benchmark)
- [Troubleshooting](#troubleshooting)
- [Donate](#donate)

## Precompiled firmware (the easy way)

If you are not familiar with building custom firmware, visit the [releases](https://github.com/cnadler86/micropython-camera-API/releases) page to download firmware that suits your board. **There are over 20 precompiled board images with the latest micropython!**
To flash the firmware, you can use for example [https://esp.huhn.me/](https://esp.huhn.me/).

These firmware binaries also include the [mp_jpeg modul](https://github.com/cnadler86/mp_jpeg) to encode/decode JPEGs.

## Using the API

### Importing the camera module

There general way of using the api is as follow:

```python
from camera import Camera, GrabMode, PixelFormat, FrameSize, GainCeiling
```

There is also a camera class with asyncio support! To use it, you need to import from the acamera package:

```python
from acamera import Camera, GrabMode, PixelFormat, FrameSize, GainCeiling
```

### Creating a camera object

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
- vsync_pin: VSYNC pin
- href_pin: HREF pin
- sda_pin: SDA pin
- scl_pin: SCL pin
- xclk_pin: XCLK pin ( set to -1, if you have an external clock source)
- xclk_freq: XCLK frequency in Hz (consult the camera sensor specification)
- powerdown_pin: Powerdown pin (set to -1 if not used)
- reset_pin: Reset pin (set to -1 if not used)
- pixel_format: Pixel format as PixelFormat
- frame_size: Frame size as FrameSize
- jpeg_quality: JPEG quality
- fb_count: Frame buffer count
- grab_mode: Grab mode as GrabMode
- init: Initialize camera at construction time (default: True)

**Default values:**

The following keyword arguments have default values:

- xclk_freq: 20MHz    // Default for OV2640 (normally either 10 MHz or 20 MHz). Plase adapt it to your camera sensor.
- frame_size: QQVGA
- pixel_format: RGB565
- jpeg_quality: 85    // Quality of JPEG output in percent. Higher means higher quality.
- powerdown_pin and reset_pin: -1 ( = not used/available/needed)
- fb_count:
  - 2 for ESP32S3 boards
  - 1 for all other
- grab_mode:
  - LATEST for ESP32S3 boards
  - WHEN_EMPTY for all other

### Initializing the camera

The camera initializes when constructing the camera object per default. If you set init=False during construction, you need to call the init method manually:

```python
cam.init()
```

Note that most of the camera seeting can only be set or aquired after initialization.

### Capture image

The general way of capturing an image is calling the `capture` method:

```python
img = cam.capture()
```

Each time you call the method, you will receive a new frame as memoryview.
You can convert it to bytes and free the memoryview buffer, so a new frame can be pushed to it. This will reduce the image latency but need more RAM. (see [freeing the buffer](#freeing-the-buffer))

The probably better way of capturing an image would be in an asyncio-loop:

```python
img = await cam.acapture() #To access this method, you need to import from acamera
```

Please consult the [asyncio documentation](https://docs.micropython.org/en/latest/library/asyncio.html), if you have questions on this.

### Camera reconfiguration

```python
cam.reconfigure(pixel_format=PixelFormat.JPEG,frame_size=FrameSize.QVGA,grab_mode=GrabMode.LATEST, fb_count=2)
```

Keyword arguments for reconfigure

- frame_size: Frame size as FrameSize (optional)
- pixel_format: Pixel format as PixelFormat(optional)
- grab_mode: Grab mode as GrabMode (optional)
- fb_count: Frame buffer count (optional)

### Freeing the buffer

This is optional, but can reduce the latency of capturing an image in some cases (especially with fb_count = 1)..

```python
Img = bytes(cam.capture())  #Create a new bytes object from the memoryview (because we want to free it afterwards)
cam.free_buffer() # This will free the captured image or in other words "deleting"" the memoryview 
```

### Is frame available

```python
Img = bytes(cam.capture())
cam.free_buffer()
while not cam.frame_available():
  <do some other stuff>
print('The frame is available now. You can grab the image by the capture method =)')
```

This gives you the possibility of creating an asynchronous application without using asyncio.

### Additional methods and examples

Here are just a few examples:

```python
cam.quality = 90  # The quality goes from 0% to 100%, meaning 100% is the highest but has probably no compression
print("cam.brightness =", cam.brightness)
camera.vflip = True #Enable vertical flip
```

See autocompletions in Thonny in order to see the list of methods.
If you want more insights in the methods and what they actually do, you can find a very good documentation [in circuitpython](https://docs.circuitpython.org/en/latest/shared-bindings/espcamera/index.html).
Note: "get_" and "set_" prefixed methods are deprecated and will be removed in a future release.

Take also a look in the examples folder.

To get the version of the camera driver used:

```python
import camera
vers = camera.Version()
```

### I2C Integration

The camera uses I2C (SCCB protocol) to communicate with the camera sensor. You can share this I2C bus with other devices by passing an external I2C object (SoftI2C not supported) to the camera:

#### Sharing I2C with Camera

```python
import machine

# Create your own I2C object first
i2c = machine.I2C(0, scl=22, sda=21, freq=400000)

# Pass it to the camera (no need for sda_pin/scl_pin)
cam = camera.Camera(i2c=i2c, data_pins=..., pclk_pin=..., ...)

# The same I2C object can be used for other devices on the same bus!
devices = i2c.scan()
print(f"I2C devices found: {devices}")

# You can communicate with other I2C devices while camera is running
i2c.writeto(0x42, b'\x00\x01')  # Write to another device

# Camera sensor communication works too
cam.saturation = 1  # Uses the shared I2C bus
```

#### Alternative: Camera Creates Its Own I2C (Default)

```python
# Camera creates and manages its own I2C internally
cam = camera.Camera(sda_pin=21, scl_pin=22, ...)

# In this mode, you cannot share I2C with other devices
# Use the first method if you need to share I2C
```

### Additional information

The firmware images support the following cameras out of the box, but is therefore big: OV7670, OV7725, OV2640, OV3660, OV5640, NT99141, GC2145, GC032A, GC0308, BF3005, BF20A6, SC030IOT

## Build your custom firmware

### Setting up the build environment (DIY method)

To build the project, follow these instructions:

- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.2.3/esp32/get-started/index.html): I used version 5.2.3, but it might work with other versions (see notes).
- Clone the micropython repo and this repo in a folder, e.g. "MyESPCam". MicroPython version 1.24 or higher is required (at least commit 92484d8).
- You will have to add the ESP32-Camera driver. To do this, add the following to the respective idf_component.yml file (e.g. in micropython/ports/esp32/main/idf_component.yml):

```yml
  espressif/esp32-camera:
    git: https://github.com/cnadler86/esp32-camera.git
```

### Add camera configurations to your board (optional, but recommended)

#### Supported camera models

This project supports various boards with camera interface out of the box. You typically only need to add a single line to your board config file ("mpconfigboard.h).
Example:

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
- MICROPY_CAMERA_MODEL_M5STACK_ATOM_S3R - [M5Stack Atom S3R](https://docs.m5stack.com/en/core/AtomS3R-M12)
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
#define MICROPY_CAMERA_PIN_SIOD     (40)        // SDA
#define MICROPY_CAMERA_PIN_SIOC     (39)        // SCL
#define MICROPY_CAMERA_XCLK_FREQ    (20000000)  // Frequencies are normally either 10 MHz or 20 MHz
#define MICROPY_CAMERA_FB_COUNT     (2)         // The value is between 1 (slow) and 2 (fast, but more load on CPU and more ram usage)
#define MICROPY_CAMERA_JPEG_QUALITY (85)        // Quality of JPEG output in percent. Higher means higher quality.
#define MICROPY_CAMERA_GRAB_MODE    (1)         // 0=WHEN_EMPTY (might have old data, but less resources), 1=LATEST (best, but more resources)

```

#### Customize additional camera settings

If you want to customize additional camera setting or reduce the firmware size by removing support for unused camera sensors, then take a look at the kconfig file of the esp32-camera driver and specify these on the sdkconfig file of your board.

#### (Optional) Add the mp_jpeg module

If you also want to include the [mp_jpeg module](https://github.com/cnadler86/mp_jpeg) in your build, clone the mp_jpeg repo at the same level and folder as the mp_camera_api repo and meet the requirements from the mp_jpeg repo.

### Build the API

To build the project, just use the buils script with the path to your micropython folder:

```bash
./build.sh -m path/to/micropython -b ESP32_GENERIC_S3
```

Use `./build.sh -h` to see all available options.
If you experience problems, visit [MicroPython external C modules](https://docs.micropython.org/en/latest/develop/cmodules.html).

## Notes

- For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has significantly improved, but JPEG mode always gives better frame rates.
- The OV5640 pinout is compatible with boards designed for the OV2640 but the voltage supply is too high for the internal 1.5V regulator, so the camera overheats unless a heat sink is applied. For recording purposes the OV5640 should only be used with an ESP32S3 board. Frame sizes above FHD framesize should only be used for still images due to memory limitations.
- If your target board is a ESP32, I recommend using IDF >= 5.2, since older versions may lead to IRAM overflow during build. Alternatively you can modify your sdkconfig-file (see [issue #1](https://github.com/cnadler86/micropython-camera-API/issues/1)).
- The driver requires PSRAM to be installed and activated.
- Most of the precompiled firmware images are untested, but the only difference between them are the target architecture and pin definitions, so they should work out of the box. If not, please raise an issue.
- Every sensor has its own ranges and settings, please consult the respective specifications.

## Benchmark

I didn't use a calibrated oscilloscope, but here is a FPS benchmark with my ESP32S3 (xclk_freq = 20MHz, GrabMode=LATEST, fb_count = 1, jpeg_quality=85%) and OV2640.
Using fb_count=2 theoretically can double the FPS (see JPEG with fb_count=2). This might also apply for other PixelFormats.

| Frame Size | GRAYSCALE | RGB565 | YUV422 | JPEG   | JPEG (fb=2) |
|------------|-----------|--------|--------|--------|-------------|
| R96X96     | 12.5      | 12.5   | 12.5   | No img | No img      |
| QQVGA      | 12.5      | 12.5   | 12.5   | 25     | 50          |
| QCIF       | 11        | 11     | 11.5   | 25     | 50          |
| HQVGA      | 12.5      | 12.5   | 12.5   | 25     | 50          |
| R240X240   | 12.5      | 12.5   | 11.5   | 25     | 50          |
| QVGA       | 12        | 11     | 12     | 25     | 50          |
| CIF        | 12.5      | No img | No img | 6.3    | 12.5        |
| HVGA       | 3         | 3      | 2.5    | 12.5   | 25          |
| VGA        | 3         | 3      | 3      | 12.5   | 25          |
| SVGA       | 3         | 3      | 3      | 12.5   | 25          |
| XGA        | No img    | No img | No img | 6.3    | 12.5        |
| HD         | No img    | No img | No img | 6.3    | 12.5        |
| SXGA       | 2         | 2      | 2      | 6.3    | 12.5        |
| UXGA       | No img    | No img | No img | 6.3    | 12.5        |

## Troubleshooting

You can find information on the following sites:

- [ESP-FAQ](https://docs.espressif.com/projects/esp-faq/en/latest/application-solution/camera-application.html)
- [ChatGPT](https://chatgpt.com/)
- [Issues in here](https://github.com/cnadler86/micropython-camera-API/issues?q=is%3Aissue)

## Donate

If you enjoy this work and would like to share your enjoyment, please feel free to [donate](https://github.com/sponsors/cnadler86?frequency=one-time) and contribute to the project. Thanks! :blush:
