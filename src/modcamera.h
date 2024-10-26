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
#include "camera_pins.h"

#if defined (MICROPY_CAMERA_PIN_SIOD) && defined (MICROPY_CAMERA_PIN_SIOC) && defined (MICROPY_CAMERA_PIN_D0) && defined (MICROPY_CAMERA_PIN_D1) && defined (MICROPY_CAMERA_PIN_D2) && \
defined (MICROPY_CAMERA_PIN_D3) && defined (MICROPY_CAMERA_PIN_D4) && defined (MICROPY_CAMERA_PIN_D5) && defined (MICROPY_CAMERA_PIN_D6) && defined (MICROPY_CAMERA_PIN_D7) && \
defined (MICROPY_CAMERA_PIN_PCLK) && defined (MICROPY_CAMERA_PIN_VSYNC) && defined (MICROPY_CAMERA_PIN_HREF) &&  defined (MICROPY_CAMERA_PIN_XCLK)
#define MICROPY_CAMERA_ALL_REQ_PINS_DEFINED (1)
#endif

#ifndef MICROPY_CAMERA_PIN_PWDN
#define MICROPY_CAMERA_PIN_PWDN    (-1)
#endif

#ifndef MICROPY_CAMERA_PIN_RESET
#define MICROPY_CAMERA_PIN_RESET   (-1)
#endif

#ifndef MICROPY_CAMERA_XCLK_FREQ
#define MICROPY_CAMERA_XCLK_FREQ   (20)
#endif

#if !defined (MICROPY_CAMERA_GRAB_MODE) && defined (CONFIG_IDF_TARGET_ESP32S3)
#define MICROPY_CAMERA_GRAB_MODE     CAMERA_GRAB_LATEST
#elif !defined(MICROPY_CAMERA_GRAB_MODE)
#define MICROPY_CAMERA_GRAB_MODE     CAMERA_GRAB_WHEN_EMPTY
#endif

#if !defined (MICROPY_CAMERA_FB_COUNT) && defined (CONFIG_IDF_TARGET_ESP32S3)
#define MICROPY_CAMERA_FB_COUNT     (2)
#elif !defined(MICROPY_CAMERA_FB_COUNT)
#define MICROPY_CAMERA_FB_COUNT     (1)
#endif

#ifndef MICROPY_CAMERA_DEFAULT_FRAME_SIZE
#define MICROPY_CAMERA_DEFAULT_FRAME_SIZE FRAMESIZE_QQVGA
#endif

#ifndef MICROPY_CAMERA_DEFAULT_PIXEL_FORMAT
#define MICROPY_CAMERA_DEFAULT_PIXEL_FORMAT PIXFORMAT_RGB565
#endif

#ifndef MICROPY_CAMERA_JPEG_QUALITY
#define MICROPY_CAMERA_JPEG_QUALITY (85)
#endif

//Supported Camera sensors
#ifndef CONFIG_OV2640_SUPPORT
#define CONFIG_OV2640_SUPPORT 1
#endif

#if !defined(CONFIG_OV5640_SUPPORT) && defined(CONFIG_IDF_TARGET_ESP32S3)
#define CONFIG_OV5640_SUPPORT 1
#endif

typedef pixformat_t  hal_camera_pixformat_t;
typedef framesize_t  hal_camera_framesize_t;
typedef camera_fb_location_t hal_camera_fb_location_t;
typedef camera_grab_mode_t hal_camera_grabmode_t;
typedef gainceiling_t hal_camera_gainceiling_t;

typedef struct hal_camera_obj {
    mp_obj_base_t       base;
    camera_config_t     camera_config;
    bool                initialized;
    camera_fb_t         *captured_buffer;
} hal_camera_obj_t;

#endif // CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

typedef hal_camera_obj_t mp_camera_obj_t;

typedef hal_camera_pixformat_t  mp_camera_pixformat_t;
typedef hal_camera_framesize_t  mp_camera_framesize_t;
typedef hal_camera_grabmode_t mp_camera_grabmode_t;
typedef hal_camera_gainceiling_t mp_camera_gainceiling_t;
// typedef hal_camera_fb_location_t mp_camera_fb_location_t; //not used at the moment, but might be used in the future

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
 * @param fb_count Number of framebuffers.
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
    int8_t fb_count,
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
 * @brief Returns true, if camera is initialized.
 * 
 * @param self Pointer to the camera object.
 */
extern bool mp_camera_hal_initialized(mp_camera_obj_t *self);

/**
 * @brief Reconfigures the camera hardware abstraction layer.
 * 
 * @param self Pointer to the camera object.
 * @param frame_size Frame size.
 * @param pixel_format Pixel format.
 * @param grab_mode Grab mode.
 * @param fb_count Number of framebuffers.
 */
extern void mp_camera_hal_reconfigure(mp_camera_obj_t *self, mp_camera_framesize_t frame_size, mp_camera_pixformat_t pixel_format, mp_camera_grabmode_t grab_mode, mp_int_t fb_count);

/**
 * @brief Captures an image and returns it as mp_obj_t (e.g. mp_obj_new_memoryview).
 * 
 * @param self Pointer to the camera object.
 * @param out_format Output pixelformat format.
 * @return Captured image as micropython object.
 */
extern mp_obj_t mp_camera_hal_capture(mp_camera_obj_t *self, int8_t out_format);

/**
 * @brief Table mapping pixel formats API to their corresponding values at HAL.
 * @details Needs to be defined in the port-specific implementation.
 */
extern const mp_rom_map_elem_t mp_camera_hal_pixel_format_table[5];

/**
 * @brief Table mapping frame sizes API to their corresponding values at HAL.
 * @details Needs to be defined in the port-specific implementation.
 */
extern const mp_rom_map_elem_t mp_camera_hal_frame_size_table[22];

/**
 * @brief Table mapping gainceiling API to their corresponding values at HAL.
 * @details Needs to be defined in the port-specific implementation.
 */
extern const mp_rom_map_elem_t mp_camera_hal_gainceiling_table[7];

/**
 * @brief Table mapping grab mode API to their corresponding values at HAL.
 * @details Needs to be defined in the port-specific implementation.
 */
extern const mp_rom_map_elem_t mp_camera_hal_grab_mode_table[2];


#define DECLARE_CAMERA_HAL_GET(type, name) \
    extern type mp_camera_hal_get_##name(mp_camera_obj_t *self);

#define DECLARE_CAMERA_HAL_SET(type, name) \
    extern void mp_camera_hal_set_##name(mp_camera_obj_t *self, type value);

/**
 * @brief Helper functions to get and set sensor properties.
 * @details The functions are used to get and set sensor properties in the camera object.
 * Needs to be defined in the port-specific implementation.
 */
#define DECLARE_CAMERA_HAL_GETSET(type, name) \
    DECLARE_CAMERA_HAL_GET(type, name) \
    DECLARE_CAMERA_HAL_SET(type, name)

DECLARE_CAMERA_HAL_GETSET(bool, aec2)
DECLARE_CAMERA_HAL_GETSET(int, aec_value)
DECLARE_CAMERA_HAL_GETSET(int, ae_level)
DECLARE_CAMERA_HAL_GETSET(int, agc_gain)
DECLARE_CAMERA_HAL_GETSET(bool, awb_gain)
DECLARE_CAMERA_HAL_GETSET(int, brightness)
DECLARE_CAMERA_HAL_GETSET(bool, bpc)
DECLARE_CAMERA_HAL_GETSET(bool, colorbar)
DECLARE_CAMERA_HAL_GETSET(int, contrast)
DECLARE_CAMERA_HAL_GETSET(bool, dcw)
DECLARE_CAMERA_HAL_GETSET(int, denoise)
DECLARE_CAMERA_HAL_GETSET(bool, exposure_ctrl)
DECLARE_CAMERA_HAL_GETSET(mp_camera_gainceiling_t, gainceiling)
DECLARE_CAMERA_HAL_GETSET(bool, gain_ctrl)
DECLARE_CAMERA_HAL_GETSET(bool, hmirror)
DECLARE_CAMERA_HAL_GETSET(bool, lenc)
DECLARE_CAMERA_HAL_GETSET(int, quality)
DECLARE_CAMERA_HAL_GETSET(bool, raw_gma)
DECLARE_CAMERA_HAL_GETSET(int, saturation)
DECLARE_CAMERA_HAL_GETSET(int, sharpness)
DECLARE_CAMERA_HAL_GETSET(int, special_effect)
DECLARE_CAMERA_HAL_GETSET(bool, vflip)
DECLARE_CAMERA_HAL_GETSET(int, wb_mode)
DECLARE_CAMERA_HAL_GETSET(bool, whitebal)
DECLARE_CAMERA_HAL_GETSET(bool, wpc)

DECLARE_CAMERA_HAL_GET(int, address)
DECLARE_CAMERA_HAL_GET(int, fb_count)
DECLARE_CAMERA_HAL_GETSET(mp_camera_framesize_t, frame_size)
DECLARE_CAMERA_HAL_GET(camera_grab_mode_t, grab_mode)
DECLARE_CAMERA_HAL_GET(mp_camera_framesize_t, max_frame_size)
DECLARE_CAMERA_HAL_GET(mp_camera_pixformat_t, pixel_format)
DECLARE_CAMERA_HAL_GET(int, pixel_height)
DECLARE_CAMERA_HAL_GET(int, pixel_width)
DECLARE_CAMERA_HAL_GET(const char *, sensor_name)
DECLARE_CAMERA_HAL_GET(bool, supports_jpeg)

#endif // MICROPY_INCLUDED_MODCAMERA_H