# IDF component wrapper for micropython-camera-API user module
# This allows the IDF component manager to process idf_component.yml
# The actual MicroPython module is built via micropython.cmake

idf_component_register(
    REQUIRES esp32-camera
)
