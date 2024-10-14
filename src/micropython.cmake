# `py.cmake` for `micropy_gather_target_properties` macro usage
include(${MICROPY_DIR}/py/py.cmake)

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

if (MICROPY_CAMERA_MODEL)
    target_compile_definitions(usermod_mp_camera INTERFACE MICROPY_CAMERA_MODEL_${MICROPY_CAMERA_MODEL}=1)
endif()

target_link_libraries(usermod INTERFACE usermod_mp_camera)

micropy_gather_target_properties(usermod_mp_camera)