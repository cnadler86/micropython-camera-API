# ESP-IDF-Komponentenregistrierung
idf_component_register(
    SRCS modcamera.c modcamera_api.c
    INCLUDE_DIRS "."
)

# Überprüfe, ob sdkconfig vorhanden ist, um benutzerdefinierte Konfiguration zu verwenden
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig")
    message(STATUS "Using custom sdkconfig")
    set(SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig")
endif()

# Micropython usermod Konfiguration
target_sources(${COMPONENT_LIB} PRIVATE modcamera.c modcamera_api.c)
target_include_directories(${COMPONENT_LIB} PRIVATE ${CMAKE_CURRENT_LIST_DIR})