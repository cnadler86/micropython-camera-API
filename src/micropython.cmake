# Include the py.cmake for additional macro micropy_gather_target_properties usage
include(${MICROPY_DIR}/py/py.cmake)

# Create an INTERFACE library for the camera module and sdd source files to the library
add_library(usermod_mp_camera INTERFACE)
target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/modcamera_api.c
)

if(NOT ESP32_CAMERA_DIR)
    set(ESP32_CAMERA_DIR "${IDF_PATH}/components/esp32-camera")
endif()

message("Camera Dir: ${ESP32_CAMERA_DIR}")

if(EXISTS "${ESP32_CAMERA_DIR}")
    target_include_directories(usermod_mp_camera INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${ESP32_CAMERA_DIR}/driver/include
        ${ESP32_CAMERA_DIR}/driver/private_include
        ${ESP32_CAMERA_DIR}/conversions/include
        ${ESP32_CAMERA_DIR}/conversions/private_include
        ${ESP32_CAMERA_DIR}/sensors/private_include
    )
else()
    target_include_directories(usermod_mp_camera INTERFACE
        ${CMAKE_CURRENT_LIST_DIR})
endif()

# Add the include directories to the target
target_include_directories(usermod_mp_camera INTERFACE ${MOD_INCLUDES})

# Add compile definitions if MICROPY_CAMERA_MODEL is defined
if (MICROPY_CAMERA_MODEL)
    target_compile_definitions(usermod_mp_camera INTERFACE MICROPY_CAMERA_MODEL_${MICROPY_CAMERA_MODEL}=1)
endif()

# Add compile definitions if MP_CAMERA_DRIVER_VERSION is defined
if (MP_CAMERA_DRIVER_VERSION)
    target_compile_definitions(usermod_mp_camera INTERFACE MP_CAMERA_DRIVER_VERSION=\"${MP_CAMERA_DRIVER_VERSION}\")
endif()

# Link the usermod_mp_camera library to the main usermod target
target_link_libraries(usermod INTERFACE usermod_mp_camera)

micropy_gather_target_properties(usermod_mp_camera)
