/*
 * The MIT License (MIT)
 *
 * Code based on circuitpython camera API by Jeff Epler
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
 * Adaptation to MicroPython by Christopher Nadler
 * Copyright (c) 2024 Christopher Nadler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#ifndef MICROPY_INCLUDED_MODCAMERA_H
#define MICROPY_INCLUDED_MODCAMERA_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"   //TODO: Maybe we can add here MICROPY_PY_MACHINE_CAMERA (0) ?
#include "py/runtime.h"
#include "py/obj.h"

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// ESP32-Camera specifics-> could go in separate header file, if this project starts implementing more ports.

#include "esp_camera.h"
#include "sensor.h"

#ifndef MICROPY_CAMERA_PIN_PWDN
#define MICROPY_CAMERA_PIN_PWDN    32
#endif

#ifndef MICROPY_CAMERA_PIN_RESET
#define MICROPY_CAMERA_PIN_RESET   -1
#endif

#ifndef MICROPY_CAMERA_PIN_SIOD
#define MICROPY_CAMERA_PIN_SIOD    26
#endif

#ifndef MICROPY_CAMERA_PIN_SIOC
#define MICROPY_CAMERA_PIN_SIOC    27
#endif

#ifndef MICROPY_CAMERA_PIN_D0
#define MICROPY_CAMERA_PIN_D0   5
#define MICROPY_CAMERA_PIN_D1   18
#define MICROPY_CAMERA_PIN_D2   19
#define MICROPY_CAMERA_PIN_D3   21
#define MICROPY_CAMERA_PIN_D4   36
#define MICROPY_CAMERA_PIN_D5   39
#define MICROPY_CAMERA_PIN_D6   34
#define MICROPY_CAMERA_PIN_D7   35
#endif

#ifndef MICROPY_CAMERA_PIN_VSYNC
#define MICROPY_CAMERA_PIN_VSYNC   25
#endif

#ifndef MICROPY_CAMERA_PIN_HREF
#define MICROPY_CAMERA_PIN_HREF    23
#endif

#ifndef MICROPY_CAMERA_PIN_PCLK
#define MICROPY_CAMERA_PIN_PCLK    22
#endif

#ifndef MICROPY_CAMERA_PIN_XCLK
#define MICROPY_CAMERA_PIN_XCLK     0
#endif

#ifndef MICROPY_CAMERA_XCLK_FREQ
#define MICROPY_CAMERA_XCLK_FREQ     10
#endif

#ifndef MICROPY_CAMERA_DEFAULT_FRAME_SIZE
#define MICROPY_CAMERA_DEFAULT_FRAME_SIZE FRAMESIZE_QQVGA
#endif

#ifndef MICROPY_CAMERA_DEFAULT_PIXEL_FORMAT
#define MICROPY_CAMERA_DEFAULT_PIXEL_FORMAT PIXFORMAT_RGB565
#endif

#ifndef MICROPY_CAMERA_GRAB_MODE
#define MICROPY_CAMERA_GRAB_MODE CAMERA_GRAB_WHEN_EMPTY
#endif

#ifndef MICROPY_CAMERA_FB_COUNT
#define MICROPY_CAMERA_FB_COUNT 1
#endif

#ifndef MICROPY_CAMERA_JPEG_QUALITY
#define MICROPY_CAMERA_JPEG_QUALITY 15
#endif

//Supported Camera sensors
#ifndef CONFIG_OV2640_SUPPORT
#define CONFIG_OV2640_SUPPORT 1
#endif

#ifndef CONFIG_OV5640_SUPPORT
#define CONFIG_OV5640_SUPPORT 1
#endif

typedef camera_config_t mp_camera_config_t;
typedef pixformat_t  mp_camera_pixformat_t;
typedef framesize_t  mp_camera_framesize_t;
typedef camera_fb_location_t mp_camera_fb_location_t;
typedef camera_grab_mode_t mp_camera_grabmode_t;
typedef gainceiling_t mp_camera_gainceiling_t;
typedef camera_fb_t mp_camera_fb_t;

typedef struct mp_camera_obj {
    mp_obj_base_t       base;
    mp_camera_config_t  camera_config;
    bool                initialized;
    mp_camera_fb_t      *capture_buffer;
} mp_camera_obj_t;

#endif // CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

//TODO: add help
extern const mp_rom_map_elem_t mp_camera_hal_frame_size_table[22];
extern const mp_rom_map_elem_t mp_camera_hal_pixel_format_table[4];

// TODO:    Define how to integrate external time source in constructor (e.g. in ESP is LED-Timer).
/**
 * @brief Constructs the camera hardware abstraction layer.
 * @details The Port-plattform shall define a default pwm-time source and also frame buffer location (no input)
 * 
 * @param self Pointer to the camera object.
 * @param data_pins Array of data pins.
 * @param external_clock_pin External clock pin.
 * @param pixel_clock_pin Pixel clock pin.
 * @param vsync_pin VSYNC pin.
 * @param href_pin HREF pin.
 * @param powerdown_pin Power down pin.
 * @param reset_pin Reset pin.
 * @param sccb_sda_pin SCCB SDA pin.
 * @param sccb_scl_pin SCCB SCL pin.
 * @param xclk_freq_hz External clock frequency in Hz.
 * @param pixel_format Pixel format.
 * @param frame_size Frame size.
 * @param jpeg_quality JPEG quality.
 * @param framebuffer_count Number of framebuffers.
 * @param grab_mode Grab mode (single-shot/when-empty OR continous/latest)
 */
extern void mp_camera_hal_construct(
    mp_camera_obj_t *self,
    int8_t data_pins[8],
    int8_t external_clock_pin,
    int8_t pixel_clock_pin,
    int8_t vsync_pin,
    int8_t href_pin,
    int8_t powerdown_pin,
    int8_t reset_pin,
    int8_t sccb_sda_pin,
    int8_t sccb_scl_pin,
    int32_t xclk_freq_hz,
    mp_camera_pixformat_t pixel_format,
    mp_camera_framesize_t frame_size,
    int8_t jpeg_quality,
    int8_t framebuffer_count,
    mp_camera_grabmode_t grab_mode);

/**
 * @brief Initializes the camera hardware abstraction layer. Will be called during API constructor
 * 
 * @param self Pointer to the camera object.
 */
extern void mp_camera_hal_init(mp_camera_obj_t *self); //since we are not passing handles at construction, init() is used to create those handles

/**
 * @brief Deinitializes the camera hardware abstraction layer.
 * 
 * @param self Pointer to the camera object.
 */
extern void mp_camera_hal_deinit(mp_camera_obj_t *self);

/**
 * @brief Reconfigures the camera hardware abstraction layer.
 * 
 * @param self Pointer to the camera object.
 * @param frame_size Frame size.
 * @param pixel_format Pixel format.
 * @param grab_mode Grab mode.
 * @param framebuffer_count Number of framebuffers.
 */
extern void mp_camera_hal_reconfigure(mp_camera_obj_t *self, mp_camera_framesize_t frame_size, mp_camera_pixformat_t pixel_format, mp_camera_grabmode_t grab_mode, mp_int_t framebuffer_count);

/**
 * @brief Captures an image as mp_obj_t.
 * 
 * @param self Pointer to the camera object.
 * @param timeout_ms Timeout in milliseconds.
 * @return Captured image as micropython object.
 */
extern mp_obj_t mp_camera_hal_capture(mp_camera_obj_t *self, int timeout_ms);

/**
 * @brief Table mapping pixel formats to their corresponding values.
 * @details Needs to be defined in the port-specific implementation.
 */
extern const mp_rom_map_elem_t mp_camera_hal_pixel_format_table[4];

/**
 * @brief Table mapping frame sizes to their corresponding values.
 * @details Needs to be defined in the port-specific implementation.
 */
extern const mp_rom_map_elem_t mp_camera_hal_frame_size_table[22];

//TODO: add help
extern const mp_rom_map_elem_t mp_camera_hal_gainceiling_table[7];
extern const mp_rom_map_elem_t mp_camera_hal_grab_mode_table[2];

// From here on are helper functions to get and set sensor properties
// The functions are used to get and set sensor properties in the camera object
#define DECLARE_SENSOR_GET(type, name) \
    extern type mp_camera_hal_get_##name(mp_camera_obj_t *self);

#define DECLARE_SENSOR_SET(type, name) \
    extern void mp_camera_hal_set_##name(mp_camera_obj_t *self, type value);

#define DECLARE_SENSOR_GETSET(type, name) \
    DECLARE_SENSOR_GET(type, name) \
    DECLARE_SENSOR_SET(type, name)

DECLARE_SENSOR_GET(mp_camera_pixformat_t, pixel_format)
DECLARE_SENSOR_GET(mp_camera_framesize_t, frame_size)
DECLARE_SENSOR_GET(camera_grab_mode_t, grab_mode)
DECLARE_SENSOR_GET(int, framebuffer_count)

DECLARE_SENSOR_GETSET(int, contrast)
DECLARE_SENSOR_GETSET(int, brightness)
DECLARE_SENSOR_GETSET(int, saturation)
DECLARE_SENSOR_GETSET(int, sharpness)
DECLARE_SENSOR_GETSET(int, denoise)
DECLARE_SENSOR_GETSET(mp_camera_gainceiling_t, gainceiling)
DECLARE_SENSOR_GETSET(int, quality)
DECLARE_SENSOR_GETSET(bool, colorbar)
DECLARE_SENSOR_GETSET(bool, whitebal)
DECLARE_SENSOR_GETSET(bool, gain_ctrl)
DECLARE_SENSOR_GETSET(bool, exposure_ctrl)
DECLARE_SENSOR_GETSET(bool, hmirror)
DECLARE_SENSOR_GETSET(bool, vflip)
DECLARE_SENSOR_GETSET(bool, aec2)
DECLARE_SENSOR_GETSET(bool, awb_gain)
DECLARE_SENSOR_GETSET(int, agc_gain)
DECLARE_SENSOR_GETSET(int, aec_value)
DECLARE_SENSOR_GETSET(int, special_effect)
DECLARE_SENSOR_GETSET(int, wb_mode)
DECLARE_SENSOR_GETSET(int, ae_level)
DECLARE_SENSOR_GETSET(bool, dcw)
DECLARE_SENSOR_GETSET(bool, bpc)
DECLARE_SENSOR_GETSET(bool, wpc)
DECLARE_SENSOR_GETSET(bool, raw_gma)
DECLARE_SENSOR_GETSET(bool, lenc)

DECLARE_SENSOR_GET(int, address)
DECLARE_SENSOR_GET(const char *, sensor_name)
DECLARE_SENSOR_GET(bool, supports_jpeg)
DECLARE_SENSOR_GET(mp_camera_framesize_t, max_frame_size)
DECLARE_SENSOR_GET(int, pixel_width)
DECLARE_SENSOR_GET(int, pixel_height)

#endif // MICROPY_INCLUDED_MODCAMERA_H