# Camera API for micropython.
This is a work in progress to support cameras in different ports in micropython, starting with the ESP32-Port and omnivision cameras. The aim of this project is to implement a general API for cameras in micropython (such as circuitpython have done).
At the moment, this is a micropython user module, but it might get in the micropython repo in the future.
The API is stable and IMHO has reached a release candidate status, but it might change without previous anounce and until I make a release, I will not maintain a changelog.

## Build the API
To build the project, follow the following instructions:
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32/get-started/index.html): I used version 5.2.2, but it might work with other versions.
- [MicroPython external C modules](https://docs.micropython.org/en/latest/develop/cmodules.html). I used the actual micropython master branch (between v1.23 and bevore 1.24).
- You will have to add the ESP32-Camera driver (I used v2.0.11). Here are 3 posibilities:
    Clone the https://github.com/espressif/esp32-camera repository inside the ~/esp-idf/components folder, or
    Add a dependency on espressif/esp32-camera component (idf.py add-dependency "espressif/esp32-camera"), or
    Manually add a dependency in idf_component.yml of your respective port in the local micropython port folder.

To build the project, you could do it the following way:
clone the micropython repo and this repo in a folder, e.g. "MyESPCam"
```bash
$ . <path2esp-idf>/esp-idf/export.sh
$ cd MyESPCam/micropython/ports/esp32
$ make USER_C_MODULES=../../../../mp_camera/src/micropython.cmake BOARD=<Your-Board> clean
$ make USER_C_MODULES=../../../../mp_camera/src/micropython.cmake BOARD=<Your-Board> submodules
$ make USER_C_MODULES=../../../../mp_camera/src/micropython.cmake BOARD=<Your-Board> all
```

# Notes
Note: There are some other repositories supporting camera drivers for the esp32, but they either have only precompiled firmware for older micropython versions, or are not aiming to be a generic camera API for micropython.