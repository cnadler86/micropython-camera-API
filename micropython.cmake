include(${MICROPY_DIR}/py/py.cmake)

set(MICROPY_FROZEN_MANIFEST ${CMAKE_CURRENT_LIST_DIR}/manifest.py)

add_library(usermod_mp_camera INTERFACE)

target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/src/modcamera_api.c
)

idf_component_get_property(camera_dir esp32-camera COMPONENT_DIR)
target_include_directories(usermod_mp_camera INTERFACE
    ${camera_dir}/driver/include
)

#     # Set MP_CAMERA_DRIVER_VERSION if available
#     if(EXISTS "${ESP32_CAMERA_MANAGED_DIR}/idf_component.yml")
#         file(READ "${ESP32_CAMERA_MANAGED_DIR}/idf_component.yml" _camera_component_yml)
#         string(REGEX MATCH "version: ([0-9]+\\.[0-9]+(\\.[0-9]+)?)" _ ${_camera_component_yml})
#         if(CMAKE_MATCH_1)
#             set(MP_CAMERA_DRIVER_VERSION "${CMAKE_MATCH_1}")
#             message(STATUS "Found esp32-camera version: ${MP_CAMERA_DRIVER_VERSION}")
#         endif()
#     endif()

# Check if MP_JPEG_DIR is set or if mp_jpeg directory exists two levels up
if(DEFINED MP_JPEG_DIR AND EXISTS "${MP_JPEG_DIR}")
    message(STATUS "Using user-defined MP_JPEG_DIR: ${MP_JPEG_DIR}")
    set(MP_JPEG_SRC "${MP_JPEG_DIR}/micropython.cmake")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../mp_jpeg")
    message(STATUS "Found mp_jpeg directory at same level as the camera module")
    set(MP_JPEG_SRC "${CMAKE_CURRENT_LIST_DIR}/../mp_jpeg/micropython.cmake")
endif()

# Include mp_jpeg module if found
if(DEFINED MP_JPEG_SRC AND EXISTS "${MP_JPEG_SRC}")
    include(${MP_JPEG_SRC})
    message(STATUS "Included mp_jpeg module from: ${MP_JPEG_SRC}")
else()
    message(STATUS "mp_jpeg module not found - camera module will build without JPEG support")
endif()

# Define MICROPY_CAMERA_MODEL if specified
if (MICROPY_CAMERA_MODEL)
    message(STATUS "Using user-defined camera model: ${MICROPY_CAMERA_MODEL}")
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

# Camera module strings are not suitable for compression and cause size increase
target_compile_definitions(usermod_mp_camera INTERFACE 
    MICROPY_ROM_TEXT_COMPRESSION=0
)

# Link the camera module with the main usermod target
target_link_libraries(usermod INTERFACE usermod_mp_camera)

# Gather target properties for MicroPython build system
micropy_gather_target_properties(usermod_mp_camera)
