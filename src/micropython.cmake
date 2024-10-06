# Check if IDF_VERSION is less than 5.2.0 AND MICROPY_BOARD is ESP32_GENERIC
if (IDF_VERSION VERSION_LESS "5.3.0" AND MICROPY_TARGET STREQUAL "esp32")
    message(WARNING "You might get IRAM errors. In this case either update to IDF Version >= 5.2 OR define CONFIG_SPIRAM_CACHE_WORKAROUND=n and CONFIG_ESP32_REV_MIN_3=y in the respective sdkconfig file.")
endif()

add_library(usermod_mp_camera INTERFACE)
target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/modcamera_api.c
)
target_include_directories(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${IDF_PATH}/components/esp32-camera/driver/include
    ${IDF_PATH}/components/esp32-camera/driver/private_include
    ${IDF_PATH}/components/esp32-camera/conversions/include
    ${IDF_PATH}/components/esp32-camera/conversions/private_include
    ${IDF_PATH}/components/esp32-camera/sensors/private_include
)
target_compile_definitions(usermod_mp_camera INTERFACE)
target_link_libraries(usermod INTERFACE usermod_mp_camera)