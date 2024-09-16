# Camera API for micropython.
This is a work in progress to support cameras in different ports in micropython, starting with the ESP32-Port and omnivision cameras. The aim of this project is to implement a general API for cameras in micropython (such as circuitpython have done).
At the moment, this is a micropython user module, but it might get in the micropython repo in the future.
The API is stable and IMHO has reached a release candidate status, but it might change without previous anounce and until I make a release, I will not maintain a changelog.

## Setup build environment
To build the project, follow the following instructions:
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32/get-started/index.html): I used version 5.2.2, but it might work with other versions.
- [MicroPython external C modules](https://docs.micropython.org/en/latest/develop/cmodules.html).
- You will have to add the ESP32-Camera driver (I used v2.0.11). Here are 3 posibilities:
    Clone the https://github.com/espressif/esp32-camera repository inside the ~/esp-idf/components folder, or
    Add a dependency on espressif/esp32-camera component (idf.py add-dependency "espressif/esp32-camera"), or
    Manually add a dependency in idf_component.yml of your respective port in the local micropython port folder.
- Clone the micropython repo and this repo in a folder, e.g. "MyESPCam". I used the actual micropython master branch (between v1.23 and bevore 1.24).

## Add camera configurations to your board (Optional, but recomended)
To make things easier, add the following lines to your board config-file "mpconfigboard.h" with the respective pins and camera parameters. Otherwise you will need to pass all parameters during construction.
Dont forget the empty line at the buttom.

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
This will follow.

## Notes
Note: There are some other repositories supporting camera drivers for the esp32, but they either have only precompiled firmware for older micropython versions, or are not aiming to be a generic camera API for micropython.