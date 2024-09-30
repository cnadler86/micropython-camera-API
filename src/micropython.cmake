# ESP-IDF-Komponentenregistrierung
idf_component_register(
    INCLUDE_DIRS "."
)

# Überprüfe, ob sdkconfig vorhanden ist, um benutzerdefinierte Konfiguration zu verwenden
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