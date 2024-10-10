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
if (MP_CAMERA_MODEL)
	add_compile_definitions(MP_CAMERA_MODEL_${MP_CAMERA_MODEL}=1)
endif()