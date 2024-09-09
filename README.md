# mp_camera
Camera user module for micropython.
This is a work in progress to support cameras in different ports in micropython, starting with the ESP32-Port and omnivision cameras. The aim of this project is to implement a general API for cameras in micropython (such as circuitpython have done).
At the moment, this is a micropython user module, but it might get in the micropython repo in the future.

There are some other repositories supporting camera drivers for the esp32, but they either have only precompiled firmware for older micropython versions, or are not aiming to be a generic camera API for micropython.

To build the project, be familiar with the instructions of [MicroPython external C modules](https://docs.micropython.org/en/latest/develop/cmodules.html).
