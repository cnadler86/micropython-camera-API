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
#include "py/mphal.h"   //Maybe we can add here MICROPY_PY_MACHINE_CAMERA (0), otherwise not needed
#include "py/runtime.h"
#include "py/obj.h"

#include "esp_camera.h" //maybe driver/esp_camera.h, but don't know yet where will the driver be located in esp-idf
#include "sensor.h"

// ESP32-Camera specifics-> could go in separate header file, if this project starts implementing more ports.
// In this case Compiler-Switch shall be used.
#ifndef CONFIG_OV2640_SUPPORT
#define CONFIG_OV2640_SUPPORT 1
#endif

//NOt implemented yet
#ifndef CONFIG_OV5640_SUPPORT
#define CONFIG_OV5640_SUPPORT 0
#endif

typedef camera_config_t mp_camera_config_t;
typedef pixformat_t  mp_camera_pixformat_t;
typedef framesize_t  mp_camera_framesize_t;
typedef camera_fb_location_t mp_camera_fb_location_t;
typedef camera_grab_mode_t mp_camera_grab_mode_t;
typedef gainceiling_t mp_camera_gainceiling_t;
typedef camera_fb_t mp_camera_fb_t;

typedef struct mp_camera_obj {
    mp_obj_base_t       base;
    mp_camera_config_t  camera_config;
    bool                initialized;
    mp_camera_fb_t      *capture_buffer;
} mp_camera_obj_t;

extern const mp_rom_map_elem_t mp_camera_hal_frame_size_table[22];
extern const mp_rom_map_elem_t mp_camera_hal_pixel_format_table[4];

#ifndef MICROPY_CAMERA_DEFAULT_FRAME_SIZE
#define MICROPY_CAMERA_DEFAULT_FRAME_SIZE FRAMESIZE_QVGA
#endif

#ifndef MICROPY_CAMERA_DEFAULT_PIXEL_FORMAT
#define MICROPY_CAMERA_DEFAULT_PIXEL_FORMAT PIXFORMAT_RGB565
#endif

#ifndef MICROPY_CAMERA_DEFAULT_GRAB_MODE
#define MICROPY_CAMERA_DEFAULT_GRAB_MODE CAMERA_GRAB_WHEN_EMPTY
#endif

//END ESP32-Camera specifics

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
 * @param grab_mode Grab mode.
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
    mp_camera_grab_mode_t grab_mode);

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
extern void mp_camera_hal_reconfigure(mp_camera_obj_t *self, mp_camera_framesize_t frame_size, mp_camera_pixformat_t pixel_format, mp_camera_grab_mode_t grab_mode, mp_int_t framebuffer_count);

/**
 * @brief Captures an image as mp_obj_t.
 * 
 * @param self Pointer to the camera object.
 * @param timeout_ms Timeout in milliseconds.
 * @return Captured image as micropython object.
 */
extern mp_obj_t mp_camera_hal_capture(mp_camera_obj_t *self, int timeout_ms);

/**
 * @brief Alternative method to capture an image as mp_obj_t.
 */
extern mp_obj_t mp_camera_hal_capture2(mp_camera_obj_t *self, int timeout_ms);

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

extern const mp_rom_map_elem_t mp_camera_hal_gainceiling_table[7];
extern const mp_rom_map_elem_t mp_camera_hal_grab_mode_table[2];

// From here on are helper functions to get and set sensor properties
// The functions are used to get and set sensor properties in the camera object
#define DECLARE_SENSOR_GETSET(type, name, field_name, setter_function_name) \
    DECLARE_SENSOR_GET(type, name, field_name, setter_function_name) \
    DECLARE_SENSOR_SET(type, name, setter_function_name)

#define DECLARE_SENSOR_STATUS_GETSET(type, name, status_field_name, setter_function_name) \
    DECLARE_SENSOR_GETSET(type, name, status.status_field_name, setter_function_name)

#define DECLARE_SENSOR_STATUS_GET(type, name, status_field_name, setter_function_name) \
    DECLARE_SENSOR_GET(type, name, status.status_field_name, setter_function_name)

#define DECLARE_SENSOR_GET(type, name, status_field_name, setter_function_name) \
    extern type mp_camera_hal_get_##name(mp_camera_obj_t * self);

#define DECLARE_SENSOR_SET(type, name, setter_function_name) \
    extern void mp_camera_hal_set_##name(mp_camera_obj_t * self, type value);

DECLARE_SENSOR_GET(mp_camera_pixformat_t, pixel_format, pixformat, set_pixformat)
DECLARE_SENSOR_STATUS_GET(mp_camera_framesize_t, frame_size, framesize, set_framesize)
DECLARE_SENSOR_STATUS_GETSET(int, contrast, contrast, set_contrast);
DECLARE_SENSOR_STATUS_GETSET(int, brightness, brightness, set_brightness);
DECLARE_SENSOR_STATUS_GETSET(int, saturation, saturation, set_saturation);
DECLARE_SENSOR_STATUS_GETSET(int, sharpness, sharpness, set_sharpness);
DECLARE_SENSOR_STATUS_GETSET(int, denoise, denoise, set_denoise);
DECLARE_SENSOR_STATUS_GETSET(mp_camera_gainceiling_t, gainceiling, gainceiling, set_gainceiling);
DECLARE_SENSOR_STATUS_GETSET(int, quality, quality, set_quality);
DECLARE_SENSOR_STATUS_GETSET(bool, colorbar, colorbar, set_colorbar);
DECLARE_SENSOR_STATUS_GETSET(bool, whitebal, whitebal, set_whitebal);
DECLARE_SENSOR_STATUS_GETSET(bool, gain_ctrl, gain_ctrl, set_gain_ctrl);
DECLARE_SENSOR_STATUS_GETSET(bool, exposure_ctrl, exposure_ctrl, set_exposure_ctrl);
DECLARE_SENSOR_STATUS_GETSET(bool, hmirror, hmirror, set_hmirror);
DECLARE_SENSOR_STATUS_GETSET(bool, vflip, vflip, set_vflip);
DECLARE_SENSOR_STATUS_GETSET(bool, aec2, aec2, set_aec2);
DECLARE_SENSOR_STATUS_GETSET(bool, awb_gain, awb_gain, set_awb_gain);
DECLARE_SENSOR_STATUS_GETSET(int, agc_gain, agc_gain, set_agc_gain);
DECLARE_SENSOR_STATUS_GETSET(int, aec_value, aec_value, set_aec_value);
DECLARE_SENSOR_STATUS_GETSET(int, special_effect, special_effect, set_special_effect);
DECLARE_SENSOR_STATUS_GETSET(int, wb_mode, wb_mode, set_wb_mode);
DECLARE_SENSOR_STATUS_GETSET(int, ae_level, ae_level, set_ae_level);
DECLARE_SENSOR_STATUS_GETSET(bool, dcw, dcw, set_dcw);
DECLARE_SENSOR_STATUS_GETSET(bool, bpc, bpc, set_bpc);
DECLARE_SENSOR_STATUS_GETSET(bool, wpc, wpc, set_wpc);
DECLARE_SENSOR_STATUS_GETSET(bool, raw_gma, raw_gma, set_raw_gma);
DECLARE_SENSOR_STATUS_GETSET(bool, lenc, lenc, set_lenc);

// From camera settings
extern camera_grab_mode_t mp_camera_hal_get_grab_mode(mp_camera_obj_t *self);
extern int mp_camera_hal_get_framebuffer_count(mp_camera_obj_t *self);

// From camera_sensor_info_t
extern int mp_camera_hal_get_address(mp_camera_obj_t *self);
extern const char *mp_camera_hal_get_sensor_name(mp_camera_obj_t *self);
extern bool mp_camera_hal_get_supports_jpeg(mp_camera_obj_t *self);
extern mp_camera_framesize_t mp_camera_hal_get_max_frame_size(mp_camera_obj_t *self);
extern int mp_camera_hal_get_pixel_width(mp_camera_obj_t *self);
extern int mp_camera_hal_get_pixel_height(mp_camera_obj_t *self);

#endif // MICROPY_INCLUDED_MODCAMERA_H