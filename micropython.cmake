include(${MICROPY_DIR}/py/py.cmake)

set(MICROPY_FROZEN_MANIFEST ${CMAKE_CURRENT_LIST_DIR}/manifest.py)

add_library(usermod_mp_camera INTERFACE)

target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/src/modcamera_api.c
)

# Register dependency on esp32-camera component
# The component is managed by IDF component manager via idf_component.yml
# Add include directories directly from managed_components (they exist after Component Manager ran)
# Allow manual override via ESP32_CAMERA_DIR
if(DEFINED ESP32_CAMERA_DIR AND EXISTS "${ESP32_CAMERA_DIR}")
    message(STATUS "Using user-defined ESP32_CAMERA_DIR: ${ESP32_CAMERA_DIR}")
    set(ESP32_CAMERA_MANAGED_DIR "${ESP32_CAMERA_DIR}")
else()
    set(ESP32_CAMERA_MANAGED_DIR "${MICROPY_PORT_DIR}/managed_components/espressif__esp32-camera")
endif()

if(EXISTS "${ESP32_CAMERA_MANAGED_DIR}")
    # Add standard include directories for esp32-camera
    list(APPEND MICROPY_INC_USERMOD
        ${ESP32_CAMERA_MANAGED_DIR}/driver/include
        ${ESP32_CAMERA_MANAGED_DIR}/driver/private_include
        ${ESP32_CAMERA_MANAGED_DIR}/conversions/include
        ${ESP32_CAMERA_MANAGED_DIR}/conversions/private_include
        ${ESP32_CAMERA_MANAGED_DIR}/sensors/private_include
    )
    
    message(STATUS "Found esp32-camera at: ${ESP32_CAMERA_MANAGED_DIR}")
    
    # Link against the component library when target exists (during actual build)
    # The target doesn't exist yet during include(), but will exist during build
    if(TARGET espressif__esp32-camera)
        idf_component_get_property(esp32_camera_lib espressif__esp32-camera COMPONENT_LIB)
        target_link_libraries(usermod_mp_camera INTERFACE ${esp32_camera_lib})
    endif()
    
    # Set MP_CAMERA_DRIVER_VERSION if available
    if(EXISTS "${ESP32_CAMERA_MANAGED_DIR}/idf_component.yml")
        file(READ "${ESP32_CAMERA_MANAGED_DIR}/idf_component.yml" _camera_component_yml)
        string(REGEX MATCH "version: ([0-9]+\\.[0-9]+(\\.[0-9]+)?)" _ ${_camera_component_yml})
        if(CMAKE_MATCH_1)
            set(MP_CAMERA_DRIVER_VERSION "${CMAKE_MATCH_1}")
            message(STATUS "Found esp32-camera version: ${MP_CAMERA_DRIVER_VERSION}")
        endif()
    endif()
else()
    message(WARNING "esp32-camera component not found - component manager should have downloaded it based on idf_component.yml")
endif()

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
