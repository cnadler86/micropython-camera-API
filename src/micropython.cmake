list(APPEND SRC_CAMERA
	modcamera.c
	modcamera_api.c
)

idf_component_register(
    SRCS ${SRC_CAMERA}
    INCLUDE_DIRS "."
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig")
    message(STATUS "Using custom sdkconfig")
    set(SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig")
endif()

add_library(usermod_mp_camera INTERFACE)
target_sources(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modcamera.c
    ${CMAKE_CURRENT_LIST_DIR}/modcamera_api.c
)
target_include_directories(usermod_mp_camera INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_compile_definitions(usermod_mp_camera INTERFACE)
target_link_libraries(usermod INTERFACE usermod_mp_camera)