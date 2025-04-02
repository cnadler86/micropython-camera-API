include(${MICROPY_DIR}/py/py.cmake)

function(check_and_register_component target component)
  get_target_property(LIBRARIES ${target} INTERFACE_LINK_LIBRARIES)
  if(NOT "${LIBRARIES}" MATCHES "${component}")
    target_link_libraries(${target} INTERFACE ${component})
  endif()
endfunction()

set(MICROPY_FROZEN_MANIFEST ${CMAKE_CURRENT_LIST_DIR}/manifest.py)

add_library(usermod_mp_camera INTERFACE)

add_dependencies(usermod_mp_camera esp32_camera)

target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/modcamera_api.c
)

# Prefer esp32-camera submodule, if not manually provided
if (NOT DEFINED ESP32_CAMERA_DIR AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/../lib/esp32-camera")
    set(ESP32_CAMERA_DIR "${CMAKE_CURRENT_LIST_DIR}/../lib/esp32-camera")
    get_filename_component(ESP32_CAMERA_DIR "${ESP32_CAMERA_DIR}" ABSOLUTE)
endif()

# Use user-defined ESP32_CAMERA_DIR if provided
if(DEFINED ESP32_CAMERA_DIR AND EXISTS "${ESP32_CAMERA_DIR}")
    message(STATUS "ESP32 Camera driver directory: ${ESP32_CAMERA_DIR}")

    target_include_directories(usermod_mp_camera INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${ESP32_CAMERA_DIR}/driver/include
        ${ESP32_CAMERA_DIR}/conversions/include
        ${ESP32_CAMERA_DIR}/driver/private_include
        ${ESP32_CAMERA_DIR}/conversions/private_include
        ${ESP32_CAMERA_DIR}/sensors/private_include
    )

# If no manual directory is provided, try to fetch it from ESP-IDF (thil will probably not work until some PR get merged into the official esp32 camera component)
elseif(EXISTS ${IDF_PATH}/components/esp32-camera)
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

# Check if MP_JPEG_DIR is set or if mp_jpeg directory exists two levels up
if(DEFINED MP_JPEG_DIR AND EXISTS "${MP_JPEG_DIR}")
    message(STATUS "Using user-defined MP_JPEG_DIR: ${MP_JPEG_DIR}")
    set(MP_JPEG_SRC "${MP_JPEG_DIR}/src/micropython.cmake")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../mp_jpeg")
    message(STATUS "Found mp_jpeg directory two levels up")
    set(MP_JPEG_SRC "${CMAKE_CURRENT_LIST_DIR}/../../mp_jpeg/src/micropython.cmake")
endif()

# Add MP_JPEG_SRC cmake file to target_sources if it is defined
if(DEFINED MP_JPEG_SRC AND EXISTS "${MP_JPEG_SRC}")
    include(${MP_JPEG_SRC})
else()
    message(WARNING "MP_JPEG_SRC not found or not defined!")
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
check_and_register_component(usermod usermod_mp_camera)

# Gather target properties for MicroPython build system
micropy_gather_target_properties(usermod_mp_camera)
