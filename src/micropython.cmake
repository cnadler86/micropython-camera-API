# Include the py.cmake for additional macro micropy_gather_target_properties usage
include(${MICROPY_DIR}/py/py.cmake)

# Create an INTERFACE library for the camera module and sdd source files to the library
add_library(usermod_mp_camera INTERFACE)
target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/modcamera_api.c
)

# Dynamically gather include directories for the esp32-camera component
idf_component_get_property(CAMERA_INCLUDES esp32-camera INCLUDE_DIRS)
idf_component_get_property(CAMERA_PRIV_INCLUDES esp32-camera PRIV_INCLUDE_DIRS)
idf_component_get_property(CAMERA_DIR esp32-camera COMPONENT_DIR)

# Create a list for include directories
set(MOD_INCLUDES ${CMAKE_CURRENT_LIST_DIR})

# Add dynamically discovered include paths
if (CAMERA_INCLUDES)
    list(TRANSFORM CAMERA_INCLUDES PREPEND ${CAMERA_DIR}/)
    list(APPEND MOD_INCLUDES ${CAMERA_INCLUDES})
endif()
if (CAMERA_PRIV_INCLUDES)
    list(TRANSFORM CAMERA_PRIV_INCLUDES PREPEND ${CAMERA_DIR}/)
    list(APPEND MOD_INCLUDES ${CAMERA_PRIV_INCLUDES})
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