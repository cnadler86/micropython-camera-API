# Camera API for micropython.
This project aims to support cameras in different ports in micropython, starting with the ESP32-Port and omnivision (OV2640 & OV5640) cameras. The project implements a general API for cameras in micropython (such as circuitpython have done).
At the moment, this is a micropython user module, but it might get in the micropython repo in the future.
The API is stable, but it might change without previous anounce and until I make a release, I will not maintain a changelog.

## Setup build environment
To build the project, follow the following instructions:
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32/get-started/index.html): I used version 5.2.2, but it might work with other versions.
- Clone the micropython repo and this repo in a folder, e.g. "MyESPCam". I used the actual micropython master branch (between v1.23 and before 1.24).
- [MicroPython external C modules](https://docs.micropython.org/en/latest/develop/cmodules.html).
- You will have to add the ESP32-Camera driver (I used v2.0.11). Here are 3 posibilities:
    Clone the https://github.com/espressif/esp32-camera repository inside the ~/esp-idf/components folder, or
    Add a dependency on espressif/esp32-camera component (idf.py add-dependency "espressif/esp32-camera"), or
    Manually add a dependency in idf_component.yml of your respective port in the local micropython port folder.

## Add camera configurations to your board (Optional, but recomended)
To make things easier, add the following lines to your board config-file "mpconfigboard.h" with the respective pins and camera parameters. Otherwise you will need to pass all parameters during construction.
Don't forget the empty line at the buttom.

```
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
#define MICROPY_CAMERA_PIN_SIOD     (40)
#define MICROPY_CAMERA_PIN_SIOC     (39)
#define MICROPY_CAMERA_XCLK_FREQ    (20000000)
#define MICROPY_CAMERA_FB_COUNT     (2)
#define MICROPY_CAMERA_JPEG_QUALITY (10)
#define MICROPY_CAMERA_GRAB_MODE    (1)

```

## Build the API
To build the project, you could do it the following way:

```bash
$ . <path2esp-idf>/esp-idf/export.sh
$ cd MyESPCam/micropython/ports/esp32
$ make USER_C_MODULES=../../../../mp_camera/src/micropython.cmake BOARD=<Your-Board> clean
$ make USER_C_MODULES=../../../../mp_camera/src/micropython.cmake BOARD=<Your-Board> submodules
$ make USER_C_MODULES=../../../../mp_camera/src/micropython.cmake BOARD=<Your-Board> all
```

## Using the API
```python
from camera import Camera, GrabMode, PixelFormat, FrameSize, GainCeiling

# Camera construction and initialization
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
    jpeg_quality=15,
    fb_count=1,
    grab_mode=GrabMode.WHEN_EMPTY
)

#Camera construction using defaults (if you specified them in mpconfigboard.h)
camera = Camera()

# Capture image
img = camera.capture()

# Camera reconfiguration 
camera.reconfigure(PixelFormat.JPEG,FrameSize.QVGA,grab_mode=GrabMode.LATEST, fb_count=2)
camera.set_quality(10)
```

You can get and set sensor properties by the respective methods (e.g. camera.get_brightness() or camera.set_vflip(True). See autocompletitions in Thonny in order to see the list of methods.
If you want more insides in the methods and what they actually do, you can find a very good documentation [here](https://docs.circuitpython.org/en/latest/shared-bindings/espcamera/index.html).
Notice that for the methods in here you need to prefix a get/set, depending that you want to do.
