/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
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

#define X(enum_value) MP_REGISTER_ENUM_CONST(enum_value),
#define MP_REGISTER_ENUM_CONST_LIST(enum_list) \
    const mp_rom_map_elem_t example_module_globals_table[] = { \
        enum_list \
    }; \
    static MP_DEFINE_CONST_DICT(mp_module_example_globals, example_module_globals_table); \
    const mp_obj_module_t mp_module_example = { \
        .base = { &mp_type_module }, \
        .globals = (mp_obj_dict_t*)&mp_module_example_globals, \
    }; \
    MP_REGISTER_MODULE(MP_QSTR_example, mp_module_example, MODULE_EXAMPLE_ENABLED);

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


// TODO:    Define how to integrate external time source in constructor (e.g. in ESP is LED-Timer).
//          Now the plattform defines a default pwm-time source and also frame buffer location in its constructor
extern void machine_hw_camera_construct(
    mp_camera_obj_t *self,
    uint8_t data_pins[8],
    uint8_t external_clock_pin,
    uint8_t pixel_clock_pin,
    uint8_t vsync_pin,
    uint8_t href_pin,
    uint8_t powerdown_pin,
    uint8_t reset_pin,
    uint8_t sccb_sda_pin,
    uint8_t sccb_scl_pin,
    int xclk_freq_hz,
    mp_camera_pixformat_t pixel_format,
    mp_camera_framesize_t frame_size,
    uint8_t jpeg_quality,
    uint8_t framebuffer_count,
    mp_camera_grab_mode_t grab_mode);

extern void machine_hw_camera_init(mp_camera_obj_t *self); //since we are not passing handles at construction, init() is used to create those handles
extern void machine_hw_camera_deinit(mp_camera_obj_t *self);
extern void machine_hw_camera_reconfigure(mp_camera_obj_t *self);

extern mp_obj_t machine_hw_camera_capture(mp_camera_obj_t *self, int timeout_ms);

// From here on are helper functions to get and set sensor properties and might not be imlemented yet
#define DECLARE_SENSOR_GETSET(type, name, field_name, setter_function_name) \
    DECLARE_SENSOR_GET(type, name, field_name, setter_function_name) \
    DECLARE_SENSOR_SET(type, name, setter_function_name)

#define DECLARE_SENSOR_STATUS_GETSET(type, name, status_field_name, setter_function_name) \
    DECLARE_SENSOR_GETSET(type, name, status.status_field_name, setter_function_name)

#define DECLARE_SENSOR_STATUS_GET(type, name, status_field_name, setter_function_name) \
    DECLARE_SENSOR_GET(type, name, status.status_field_name, setter_function_name)

#define DECLARE_SENSOR_GET(type, name, status_field_name, setter_function_name) \
    extern type machine_hw_camera_get_##name(mp_camera_obj_t * self);

#define DECLARE_SENSOR_SET(type, name, setter_function_name) \
    extern void machine_hw_camera_set_##name(mp_camera_obj_t * self, type value);

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

// From settings
extern camera_grab_mode_t machine_hw_camera_get_grab_mode(mp_camera_obj_t *self);
extern int machine_hw_camera_get_framebuffer_count(mp_camera_obj_t *self);

// From camera_sensor_info_t
extern int machine_hw_camera_get_address(mp_camera_obj_t *self);
extern const char *machine_hw_camera_get_sensor_name(mp_camera_obj_t *self);
extern bool machine_hw_camera_get_supports_jpeg(mp_camera_obj_t *self);
extern mp_camera_framesize_t machine_hw_camera_get_max_frame_size(mp_camera_obj_t *self);
extern int machine_hw_camera_get_width(mp_camera_obj_t *self);
extern int machine_hw_camera_get_height(mp_camera_obj_t *self);

#endif // MICROPY_INCLUDED_MODCAMERA_H
