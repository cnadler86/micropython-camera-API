include(${MICROPY_DIR}/py/py.cmake)

# IDF version >= 5.4 not supported yet
message(STATUS "IDF_VERSION: ${IDF_VER}")
if (NOT IDF_VERSION_MINOR VERSION_LESS 4)
    message(FATAL_ERROR "ESP-IDF version >= 5.4 or later is not supported by micropython with the esp32-camera yet. Please use IDF 5.2 or 5.3")
endif()

add_library(usermod_mp_camera INTERFACE)

target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/modcamera_api.c
)

# Prefer user-defined ESP32_CAMERA_DIR if provided
if(DEFINED ESP32_CAMERA_DIR AND EXISTS "${ESP32_CAMERA_DIR}")
    message(STATUS "Using user-defined ESP32 Camera directory: ${ESP32_CAMERA_DIR}")

    target_include_directories(usermod_mp_camera INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${ESP32_CAMERA_DIR}/driver/include
        ${ESP32_CAMERA_DIR}/conversions/include
        ${ESP32_CAMERA_DIR}/driver/private_include
        ${ESP32_CAMERA_DIR}/conversions/private_include
        ${ESP32_CAMERA_DIR}/sensors/private_include
    )

# If no manual directory is provided, try to fetch it from ESP-IDF
else()
    idf_component_get_property(CAMERA_INCLUDES esp32-camera INCLUDE_DIRS)
    idf_component_get_property(CAMERA_PRIV_INCLUDES esp32-camera PRIV_INCLUDE_DIRS)
    idf_component_get_property(CAMERA_DIR esp32-camera COMPONENT_DIR)

    if(CAMERA_DIR)
        message(STATUS "Using ESP32 Camera component from ESP-IDF: ${CAMERA_DIR}")

        # Add public include directories from ESP-IDF
        if(CAMERA_INCLUDES)
            list(TRANSFORM CAMERA_INCLUDES PREPEND ${CAMERA_DIR}/)
            target_include_directories(usermod_mp_camera INTERFACE ${CAMERA_INCLUDES})
        endif()

        # Add private include directories from ESP-IDF
        if(CAMERA_PRIV_INCLUDES)
            list(TRANSFORM CAMERA_PRIV_INCLUDES PREPEND ${CAMERA_DIR}/)
            target_include_directories(usermod_mp_camera INTERFACE ${CAMERA_PRIV_INCLUDES})
        endif()
    else()
        message(WARNING "ESP32 Camera component not found in ESP-IDF!")
        target_include_directories(usermod_mp_camera PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    endif()
endif()

# Define MICROPY_CAMERA_MODEL if specified
if (MICROPY_CAMERA_MODEL)
    target_compile_definitions(usermod_mp_camera INTERFACE 
        MICROPY_CAMERA_MODEL_${MICROPY_CAMERA_MODEL}=1
    )
endif()

# Define MP_CAMERA_DRIVER_VERSION if specified
if (MP_CAMERA_DRIVER_VERSION)
    target_compile_definitions(usermod_mp_camera INTERFACE 
        MP_CAMERA_DRIVER_VERSION=\"${MP_CAMERA_DRIVER_VERSION}\"
    )
endif()

# Link the camera module with the main usermod target
target_link_libraries(usermod INTERFACE usermod_mp_camera)

# Gather target properties for MicroPython build system
micropy_gather_target_properties(usermod_mp_camera)
