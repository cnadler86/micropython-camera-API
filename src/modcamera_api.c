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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "modcamera.h"

#if MICROPY_HW_ESP_NEW_I2C_DRIVER
#include "driver/i2c_master.h"
#else
#include "driver/i2c.h"
#include "hal/i2c_ll.h"
#endif

typedef struct mp_camera_obj_t mp_camera_obj;
const mp_obj_type_t camera_type;

// Static camera object to ensure only one instance exists at a time
static mp_camera_obj_t mp_camera_singleton = { .base = { NULL } };

//Constructor
static mp_obj_t mp_camera_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_data_pins, ARG_pixel_clock_pin, ARG_vsync_pin, ARG_href_pin, ARG_sda_pin, ARG_scl_pin, ARG_xclock_pin, ARG_i2c, ARG_xclock_frequency, ARG_powerdown_pin, ARG_reset_pin, ARG_pixel_format, ARG_frame_size, ARG_jpeg_quality, ARG_fb_count, ARG_grab_mode, ARG_init, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        #ifdef MICROPY_CAMERA_ALL_REQ_PINS_DEFINED
            { MP_QSTR_data_pins, MP_ARG_OBJ | MP_ARG_KW_ONLY , { .u_obj = MP_ROM_NONE } },
            { MP_QSTR_pclk_pin, MP_ARG_INT | MP_ARG_KW_ONLY , { .u_int = MICROPY_CAMERA_PIN_PCLK } },
            { MP_QSTR_vsync_pin, MP_ARG_INT | MP_ARG_KW_ONLY , { .u_int = MICROPY_CAMERA_PIN_VSYNC } },
            { MP_QSTR_href_pin, MP_ARG_INT | MP_ARG_KW_ONLY , { .u_int = MICROPY_CAMERA_PIN_HREF } },
            { MP_QSTR_sda_pin, MP_ARG_INT | MP_ARG_KW_ONLY , { .u_int = MICROPY_CAMERA_PIN_SIOD } },
            { MP_QSTR_scl_pin, MP_ARG_INT | MP_ARG_KW_ONLY , { .u_int = MICROPY_CAMERA_PIN_SIOC } },
            { MP_QSTR_xclk_pin, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_PIN_XCLK } },
        #else
            { MP_QSTR_data_pins, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
            { MP_QSTR_pclk_pin, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
            { MP_QSTR_vsync_pin, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
            { MP_QSTR_href_pin, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
            { MP_QSTR_sda_pin, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = -1 } },
            { MP_QSTR_scl_pin, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = -1 } },
            { MP_QSTR_xclk_pin, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        #endif
        { MP_QSTR_i2c, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_xclk_freq, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_XCLK_FREQ } },
        { MP_QSTR_powerdown_pin, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_PIN_PWDN } },
        { MP_QSTR_reset_pin, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_PIN_RESET } },
        { MP_QSTR_pixel_format, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_DEFAULT_PIXEL_FORMAT } },
        { MP_QSTR_frame_size, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_DEFAULT_FRAME_SIZE } },
        { MP_QSTR_jpeg_quality, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_JPEG_QUALITY } },
        { MP_QSTR_fb_count, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_FB_COUNT } },
        { MP_QSTR_grab_mode, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = MICROPY_CAMERA_GRAB_MODE } },
        { MP_QSTR_init, MP_ARG_BOOL | MP_ARG_KW_ONLY, { .u_bool = true } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    //TODO: validate inputs
    int8_t data_pins[8];
    mp_obj_t data_pins_obj = args[ARG_data_pins].u_obj;
    if (data_pins_obj == MP_ROM_NONE) {
        #ifdef MICROPY_CAMERA_ALL_REQ_PINS_DEFINED
            if (MICROPY_CAMERA_PIN_D0 != -1) {
                data_pins[0] = MICROPY_CAMERA_PIN_D0;
                data_pins[1] = MICROPY_CAMERA_PIN_D1;
                data_pins[2] = MICROPY_CAMERA_PIN_D2;
                data_pins[3] = MICROPY_CAMERA_PIN_D3;
                data_pins[4] = MICROPY_CAMERA_PIN_D4;
                data_pins[5] = MICROPY_CAMERA_PIN_D5;
                data_pins[6] = MICROPY_CAMERA_PIN_D6;
                data_pins[7] = MICROPY_CAMERA_PIN_D7;
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("Specify a valid camera configuration"));
            }
        #endif
    } else {
        if (!mp_obj_is_type(data_pins_obj, &mp_type_list) && !mp_obj_is_type(data_pins_obj, &mp_type_bytearray)) {
            mp_raise_TypeError(MP_ERROR_TEXT("data_pins must be a list or bytearray"));
        }
        mp_uint_t data_pins_len = mp_obj_get_int(mp_obj_len_maybe(data_pins_obj));
        if (data_pins_len != 8) {
            mp_raise_ValueError(MP_ERROR_TEXT("data_pins must have 8 elements"));
        }
        for (mp_uint_t i = 0; i < 8; i++) {
            mp_obj_t item = mp_obj_subscr(data_pins_obj, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL);
            data_pins[i] = mp_obj_get_int(item);
        }
    }
    int8_t pixel_clock_pin = args[ARG_pixel_clock_pin].u_int;
    int8_t vsync_pin = args[ARG_vsync_pin].u_int;
    int8_t href_pin = args[ARG_href_pin].u_int;
    
    // Handle I2C configuration: either use I2C object or individual pins
    int8_t sda_pin = -1;
    int8_t scl_pin = -1;
    int8_t i2c_port = -1;
    mp_obj_t i2c_obj = args[ARG_i2c].u_obj;
    
    if (i2c_obj != MP_ROM_NONE) {
        // External I2C object provided - extract port number from it
        extern const mp_obj_type_t machine_i2c_type;
        if (!mp_obj_is_type(i2c_obj, &machine_i2c_type)) {
            mp_raise_TypeError(MP_ERROR_TEXT("i2c must be a machine.I2C object"));
        }
        
        // Get the I2C port from the I2C object
        // Structure matches machine_hw_i2c_obj_t from machine_i2c.c
        typedef struct _machine_hw_i2c_obj_t {
            mp_obj_base_t base;
            i2c_port_t port : 8;
            gpio_num_t scl : 8;
            gpio_num_t sda : 8;
            uint32_t freq;
            uint32_t timeout_us;
        } machine_hw_i2c_obj_t;

        machine_hw_i2c_obj_t *i2c = (machine_hw_i2c_obj_t *)MP_OBJ_TO_PTR(i2c_obj);
        i2c_port = (int8_t)i2c->port;
        sda_pin = i2c->sda;
        scl_pin = i2c->scl;
    } else {
        // Use individual pins
        sda_pin = args[ARG_sda_pin].u_int;
        scl_pin = args[ARG_scl_pin].u_int;
        
        // Validate that pins are provided (if no default pins defined)
        #ifndef MICROPY_CAMERA_ALL_REQ_PINS_DEFINED
            if (sda_pin == -1 || scl_pin == -1) {
                mp_raise_ValueError(MP_ERROR_TEXT("Either i2c object or sda_pin/scl_pin must be specified"));
            }
        #endif
    }
    
    int8_t xclock_pin = args[ARG_xclock_pin].u_int;
    int32_t xclock_frequency = args[ARG_xclock_frequency].u_int;
    if (xclock_frequency < 1000) {
        xclock_frequency = xclock_frequency * 1000000;
    }
    int8_t powerdown_pin = args[ARG_powerdown_pin].u_int;
    int8_t reset_pin = args[ARG_reset_pin].u_int;
    mp_camera_pixformat_t pixel_format = args[ARG_pixel_format].u_int;
    mp_camera_framesize_t frame_size = args[ARG_frame_size].u_int;
    int8_t jpeg_quality = args[ARG_jpeg_quality].u_int;
    if ((jpeg_quality < 0) || (jpeg_quality > 100)) {
        mp_raise_ValueError(MP_ERROR_TEXT("jpeg quality must be in range 0-100"));
    }
    int8_t fb_count = args[ARG_fb_count].u_int;
    mp_camera_grabmode_t grab_mode = args[ARG_grab_mode].u_int;
    
    // Get static singleton object
    mp_camera_obj_t *self = &mp_camera_singleton;
    
    if (self->base.type == NULL) {
        self->base.type = &camera_type;
    } else {
        mp_camera_hal_deinit(self);     // Camera already exists, deinit it before reinitializing
    }

    mp_camera_hal_construct(self, data_pins, xclock_pin, pixel_clock_pin, vsync_pin, href_pin, powerdown_pin, reset_pin, 
        sda_pin, scl_pin, i2c_port, xclock_frequency, pixel_format, frame_size, jpeg_quality, fb_count, grab_mode);

    mp_camera_hal_init(self);
    mp_hal_delay_ms(10); // Small delay to ensure I2C/SCCB is fully initialized
    if (mp_camera_hal_capture(self) == mp_const_none){
        mp_camera_hal_deinit(self);
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to capture initial frame. Construct a new object with appropriate configuration."));
    } else {
        if ( !args[ARG_init].u_bool ){
            mp_camera_hal_deinit(self);
        } else {
            mp_camera_hal_free_buffer(self);
        }
        return MP_OBJ_FROM_PTR(self);
    }
} // camera_construct

// Main methods
static mp_obj_t camera_capture(mp_obj_t self_in){
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_camera_hal_capture(self);
}
static MP_DEFINE_CONST_FUN_OBJ_1(camera_capture_obj, camera_capture);

static mp_obj_t camera_frame_available(mp_obj_t self_in){
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_camera_hal_frame_available(self);
}
static MP_DEFINE_CONST_FUN_OBJ_1(camera_frame_available_obj, camera_frame_available);

static mp_obj_t camera_free_buf(mp_obj_t self_in) {
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_camera_hal_free_buffer(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(camera_free_buf_obj, camera_free_buf);

static mp_obj_t camera_reconfigure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    enum { ARG_frame_size, ARG_pixel_format, ARG_grab_mode, ARG_fb_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_frame_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_pixel_format, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_grab_mode, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_fb_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_camera_framesize_t frame_size =
        args[ARG_frame_size].u_obj != MP_ROM_NONE
        ?  args[ARG_frame_size].u_int
        : mp_camera_hal_get_frame_size(self);
    mp_camera_pixformat_t pixel_format =
        args[ARG_pixel_format].u_obj != MP_ROM_NONE
        ?  args[ARG_pixel_format].u_int
        : mp_camera_hal_get_pixel_format(self);
    mp_camera_grabmode_t grab_mode =
        args[ARG_grab_mode].u_obj != MP_ROM_NONE
        ?  args[ARG_grab_mode].u_int
        : mp_camera_hal_get_grab_mode(self);
    mp_int_t fb_count =
        args[ARG_fb_count].u_obj != MP_ROM_NONE
        ?  args[ARG_fb_count].u_int
        : mp_camera_hal_get_fb_count(self);
    
    mp_camera_hal_reconfigure(self, frame_size, pixel_format, grab_mode, fb_count);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(camera_reconfigure_obj, 1, camera_reconfigure);

static mp_obj_t camera_init(mp_obj_t self_in) {
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_camera_hal_init(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(camera_init_obj, camera_init);

static mp_obj_t mp_camera_deinit(mp_obj_t self_in) {
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_camera_hal_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_camera_deinit_obj, mp_camera_deinit);

// Destructor
static mp_obj_t mp_camera_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_camera_deinit(args[0]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_camera___exit___obj, 4, 4, mp_camera_obj___exit__);

// Camera property functions
// Camera sensor property functions
#define CREATE_GETTER(property, get_function) \
    static mp_obj_t camera_get_##property(const mp_obj_t self_in) { \
        mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in); \
        return get_function(mp_camera_hal_get_##property(self)); \
    } \
    static MP_DEFINE_CONST_FUN_OBJ_1(camera_get_##property##_obj, camera_get_##property);

#define CREATE_SETTER(property, set_conversion) \
    static mp_obj_t camera_set_##property(const mp_obj_t self_in, const mp_obj_t arg) { \
        mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in); \
        mp_camera_hal_set_##property(self, set_conversion(arg)); \
        if (mp_camera_hal_get_##property(self) != set_conversion(arg)) { \
            mp_warning(NULL,"Failed to set " #property); \
        }; \
        return mp_const_none; \
    } \
    static MP_DEFINE_CONST_FUN_OBJ_2(camera_set_##property##_obj, camera_set_##property);

#define CREATE_GETSET_FUNCTIONS(property, get_function, set_conversion) \
    CREATE_GETTER(property, get_function) \
    CREATE_SETTER(property, set_conversion)

// Create table property entry
#define ADD_PROPERTY_TO_TABLE(property) \
    { MP_ROM_QSTR(MP_QSTR_get_##property), MP_ROM_PTR(&camera_get_##property##_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_set_##property), MP_ROM_PTR(&camera_set_##property##_obj) }

CREATE_GETSET_FUNCTIONS(frame_size, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETTER(pixel_format, mp_obj_new_int);
CREATE_GETTER(grab_mode, mp_obj_new_int);
CREATE_GETTER(fb_count, mp_obj_new_int);
CREATE_GETTER(pixel_width, mp_obj_new_int);
CREATE_GETTER(pixel_height, mp_obj_new_int);
CREATE_GETTER(max_frame_size, mp_obj_new_int);
CREATE_GETTER(sensor_name, mp_obj_new_str_from_cstr);
CREATE_GETSET_FUNCTIONS(contrast, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(brightness, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(saturation, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(sharpness, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(denoise, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(gainceiling, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(quality, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(colorbar, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(whitebal, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(gain_ctrl, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(exposure_ctrl, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(hmirror, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(vflip, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(aec2, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(awb_gain, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(agc_gain, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(aec_value, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(special_effect, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(wb_mode, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(ae_level, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(dcw, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(bpc, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(wpc, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(raw_gma, mp_obj_new_bool, mp_obj_is_true);
CREATE_GETSET_FUNCTIONS(lenc, mp_obj_new_bool, mp_obj_is_true);

//API-Tables
static const mp_rom_map_elem_t camera_camera_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reconfigure), MP_ROM_PTR(&camera_reconfigure_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&camera_capture_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame_available), MP_ROM_PTR(&camera_frame_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_free_buffer), MP_ROM_PTR(&camera_free_buf_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&camera_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_camera___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_get_pixel_format), MP_ROM_PTR(&camera_get_pixel_format_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_grab_mode), MP_ROM_PTR(&camera_get_grab_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_fb_count), MP_ROM_PTR(&camera_get_fb_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_pixel_width), MP_ROM_PTR(&camera_get_pixel_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_pixel_height), MP_ROM_PTR(&camera_get_pixel_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_max_frame_size), MP_ROM_PTR(&camera_get_max_frame_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_sensor_name), MP_ROM_PTR(&camera_get_sensor_name_obj) },
    ADD_PROPERTY_TO_TABLE(frame_size),
    ADD_PROPERTY_TO_TABLE(contrast),
    ADD_PROPERTY_TO_TABLE(brightness),
    ADD_PROPERTY_TO_TABLE(saturation),
    ADD_PROPERTY_TO_TABLE(sharpness),
    ADD_PROPERTY_TO_TABLE(denoise),
    ADD_PROPERTY_TO_TABLE(gainceiling),
    ADD_PROPERTY_TO_TABLE(quality),
    ADD_PROPERTY_TO_TABLE(colorbar),
    ADD_PROPERTY_TO_TABLE(whitebal),
    ADD_PROPERTY_TO_TABLE(gain_ctrl),
    ADD_PROPERTY_TO_TABLE(exposure_ctrl),
    ADD_PROPERTY_TO_TABLE(hmirror),
    ADD_PROPERTY_TO_TABLE(vflip),
    ADD_PROPERTY_TO_TABLE(aec2),
    ADD_PROPERTY_TO_TABLE(awb_gain),
    ADD_PROPERTY_TO_TABLE(agc_gain),
    ADD_PROPERTY_TO_TABLE(aec_value),
    ADD_PROPERTY_TO_TABLE(special_effect),
    ADD_PROPERTY_TO_TABLE(wb_mode),
    ADD_PROPERTY_TO_TABLE(ae_level),
    ADD_PROPERTY_TO_TABLE(dcw),
    ADD_PROPERTY_TO_TABLE(bpc),
    ADD_PROPERTY_TO_TABLE(wpc),
    ADD_PROPERTY_TO_TABLE(raw_gma),
    ADD_PROPERTY_TO_TABLE(lenc),
};
static MP_DEFINE_CONST_DICT(camera_camera_locals_dict, camera_camera_locals_table);

//Helper methods
static void mp_camera_hal_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_camera_hal_initialized(self)) {
        mp_printf(print, "Camera with sensor %s", mp_camera_hal_get_sensor_name(self));
    } else {
        mp_printf(print, "Camera unknown");
    }
}

//API module definition
#define MP_CREATE_CONST_TYPE(type, typename, ...) \
    MP_DEFINE_CONST_OBJ_TYPE(typename##_type, \
    MP_QSTR_##type, \
    MP_TYPE_FLAG_NONE, \
    locals_dict, &typename##_locals_dict, \
    ##__VA_ARGS__ \
    )

static MP_DEFINE_CONST_DICT(mp_camera_frame_size_locals_dict,mp_camera_hal_frame_size_table);
MP_CREATE_CONST_TYPE(FrameSize, mp_camera_frame_size);

static MP_DEFINE_CONST_DICT(mp_camera_pixel_format_locals_dict,mp_camera_hal_pixel_format_table);
MP_CREATE_CONST_TYPE(PixelFormat, mp_camera_pixel_format);

static MP_DEFINE_CONST_DICT(mp_camera_gainceiling_locals_dict,mp_camera_hal_gainceiling_table);
MP_CREATE_CONST_TYPE(GainCeiling, mp_camera_gainceiling);

static MP_DEFINE_CONST_DICT(mp_camera_grab_mode_locals_dict,mp_camera_hal_grab_mode_table);
MP_CREATE_CONST_TYPE(GrabMode, mp_camera_grab_mode);

#ifdef MP_CAMERA_DRIVER_VERSION
    static mp_obj_t mp_camera_driver_version(void) {
        return mp_obj_new_str(MP_CAMERA_DRIVER_VERSION, strlen(MP_CAMERA_DRIVER_VERSION));
    }
    static MP_DEFINE_CONST_FUN_OBJ_0(mp_camera_driver_version_obj, mp_camera_driver_version);
#endif

static const mp_rom_map_elem_t camera_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_camera) },
    { MP_ROM_QSTR(MP_QSTR_Camera),    MP_ROM_PTR(&camera_type) },
    { MP_ROM_QSTR(MP_QSTR_PixelFormat), MP_ROM_PTR(&mp_camera_pixel_format_type) },
    { MP_ROM_QSTR(MP_QSTR_FrameSize), MP_ROM_PTR(&mp_camera_frame_size_type) },
    { MP_ROM_QSTR(MP_QSTR_GainCeiling), MP_ROM_PTR(&mp_camera_gainceiling_type) },
    { MP_ROM_QSTR(MP_QSTR_GrabMode), MP_ROM_PTR(&mp_camera_grab_mode_type) },
    #ifdef MP_CAMERA_DRIVER_VERSION
        { MP_ROM_QSTR(MP_QSTR_Version), MP_ROM_PTR(&mp_camera_driver_version_obj) },
    #endif
};
static MP_DEFINE_CONST_DICT(camera_module_globals, camera_module_globals_table);

MP_DEFINE_CONST_OBJ_TYPE(
    camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_NONE,
    make_new, mp_camera_make_new,
    print, mp_camera_hal_print,
    locals_dict, &camera_camera_locals_dict
);

const mp_obj_module_t camera_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&camera_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_camera, camera_module);