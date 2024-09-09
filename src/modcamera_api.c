/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

//OPEN: check initi of camera in macro

#define CREATE_GETTER(property, get_function) \
    static mp_obj_t camera_get_##property(const mp_obj_t self_in) { \
        mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in); \
        return get_function(mp_camera_hal_get_##property(self)); \
    } \
    MP_DEFINE_CONST_FUN_OBJ_1(camera_get_##property##_obj, camera_get_##property);

#define CREATE_SETTER(property, set_conversion) \
    static mp_obj_t camera_set_##property(const mp_obj_t self_in, const mp_obj_t arg) { \
        mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in); \
        mp_camera_hal_set_##property(self, set_conversion(arg)); \
        return mp_const_none; \
    } \
    MP_DEFINE_CONST_FUN_OBJ_2(camera_set_##property##_obj, camera_set_##property);

#define CREATE_GETSET_FUNCTIONS(property, get_function, set_conversion) \
    CREATE_GETTER(property, get_function) \
    CREATE_SETTER(property, set_conversion)

#define ADD_PROPERTY_TO_LOCALS(property) \
    { MP_ROM_QSTR(MP_QSTR_get_##property), MP_ROM_PTR(&camera_get_##property##_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_set_##property), MP_ROM_PTR(&camera_set_##property##_obj) }

typedef struct mp_camera_obj_t mp_camera_obj;
const mp_obj_type_t camera_type;

//Constructor
static mp_obj_t camera_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_data_pins, ARG_pixel_clock_pin, ARG_vsync_pin, ARG_href_pin, ARG_sda_pin, ARG_scl_pin, ARG_xclock_pin, ARG_xclock_frequency, ARG_powerdown_pin, ARG_reset_pin, ARG_pixel_format, ARG_frame_size, ARG_jpeg_quality, ARG_framebuffer_count, ARG_grab_mode, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data_pins, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_pixel_clock_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_vsync_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_href_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_sda_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_scl_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_xclock_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_xclock_frequency, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 20000000L } },
        { MP_QSTR_powerdown_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_reset_pin, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
        { MP_QSTR_pixel_format, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_PTR((void *)&pixel_format_RGB565_obj) } },
        { MP_QSTR_frame_size, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_PTR((void *)&frame_size_QQVGA_obj) } },
        { MP_QSTR_jpeg_quality, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 15 } },
        { MP_QSTR_framebuffer_count, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 1 } },
        { MP_QSTR_grab_mode, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_PTR((void *)&grab_mode_WHEN_EMPTY_obj) } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    //TODO: declare variables and validate inputs


    mp_camera_obj_t *self = mp_obj_malloc_with_finaliser(mp_camera_obj_t, &camera_type);
    self->base.type = &camera_type;

    mp_camera_hal_construct(self, data_pins, pixel_clock_pin, vsync_pin, href_pin, sda_pin, scl_pin, xclock_pin, xclock_frequency, 
        powerdown_pin, reset_pin, pixel_format, frame_size, jpeg_quality, framebuffer_count, grab_mode);
    mp_camera_hal_init(self);
    (void)mp_camera_hal_capture(self, 100); //used in order to reserve a memory block for framebuffer while construction
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t camera_make_new_stub(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)all_args;
    mp_camera_obj_t *self = mp_obj_malloc_with_finaliser(mp_camera_obj_t, &camera_type);
    self->base.type = &camera_type;
    framesize_t frame_size = FRAMESIZE_HVGA;
    pixformat_t pixel_format = PIXFORMAT_JPEG;
    camera_grab_mode_t grab_mode = CAMERA_GRAB_LATEST;
    uint8_t data_pins[8] = {15,17,18,16,14,12,11,48};
    mp_camera_hal_construct(self,data_pins,
        10,
        13,
        38,
        47,
        -1,
        -1,
        40,
        39,
        20000000,
        pixel_format,
        frame_size,
        10,
        2,
        grab_mode);
    mp_camera_hal_init(self);
    (void)mp_camera_hal_capture(self, 100);
    return MP_OBJ_FROM_PTR(self);
}

// Main methods
static mp_obj_t camera_capture(size_t n_args, const mp_obj_t *args){
    //DONE
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_float_t timeout = n_args < 2 ? MICROPY_FLOAT_CONST(0.25) : mp_obj_get_float(args[1]);
    return mp_camera_hal_capture(self, timeout);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(camera_capture_obj, 1, 2, camera_capture);

static mp_obj_t camera_reconfigure(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){
    //OPEN: Validate inputs
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    enum { ARG_frame_size, ARG_pixel_format, ARG_grab_mode, ARG_framebuffer_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_frame_size, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_pixel_format, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_grab_mode, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_framebuffer_count, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    //TODO: validate and typecast values from api to hal types
    mp_camera_framesize_t frame_size =
        args[ARG_frame_size].u_obj != MP_ROM_NONE
        ?  mp_obj_get_int(args[ARG_frame_size].u_obj)
        : mp_camera_hal_get_frame_size(self);
    mp_camera_pixformat_t pixel_format =
        args[ARG_pixel_format].u_obj != MP_ROM_NONE
        ?  mp_obj_get_int(args[ARG_pixel_format].u_obj)
        : mp_camera_hal_get_pixel_format(self);
    mp_camera_grab_mode_t grab_mode =
        args[ARG_grab_mode].u_obj != MP_ROM_NONE
        ?  mp_obj_get_int(args[ARG_grab_mode].u_obj)
        : mp_camera_hal_get_grab_mode(self);
    bool framebuffer_count =
        args[ARG_framebuffer_count].u_obj != MP_ROM_NONE
        ?  mp_obj_get_int(args[ARG_framebuffer_count].u_obj)
        : mp_camera_hal_get_framebuffer_count(self);
    
    mp_camera_hal_reconfigure(self, frame_size, pixel_format, grab_mode, framebuffer_count);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(camera_reconfigure_obj, 1, camera_reconfigure);

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

static mp_obj_t mp_camera_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_camera_deinit(args[0]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_camera___exit___obj, 4, 4, mp_camera_obj___exit__);

// Camera propertiy functions
CREATE_GETSET_FUNCTIONS(contrast, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(brightness, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(saturation, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(sharpness, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
CREATE_GETSET_FUNCTIONS(denoise, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int);
// CREATE_GETSET_FUNCTIONS(gainceiling, MP_OBJ_NEW_SMALL_INT, mp_obj_get_int); //TODO
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
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&camera_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_camera___exit___obj) },
    ADD_PROPERTY_TO_LOCALS(contrast),
    ADD_PROPERTY_TO_LOCALS(brightness),
    ADD_PROPERTY_TO_LOCALS(saturation),
    ADD_PROPERTY_TO_LOCALS(sharpness),
    ADD_PROPERTY_TO_LOCALS(denoise),
    // ADD_PROPERTY_TO_LOCALS(gainceiling),
    ADD_PROPERTY_TO_LOCALS(quality),
    ADD_PROPERTY_TO_LOCALS(colorbar),
    ADD_PROPERTY_TO_LOCALS(whitebal),
    ADD_PROPERTY_TO_LOCALS(gain_ctrl),
    ADD_PROPERTY_TO_LOCALS(exposure_ctrl),
    ADD_PROPERTY_TO_LOCALS(hmirror),
    ADD_PROPERTY_TO_LOCALS(vflip),
    ADD_PROPERTY_TO_LOCALS(aec2),
    ADD_PROPERTY_TO_LOCALS(awb_gain),
    ADD_PROPERTY_TO_LOCALS(agc_gain),
    ADD_PROPERTY_TO_LOCALS(aec_value),
    ADD_PROPERTY_TO_LOCALS(special_effect),
    ADD_PROPERTY_TO_LOCALS(wb_mode),
    ADD_PROPERTY_TO_LOCALS(ae_level),
    ADD_PROPERTY_TO_LOCALS(dcw),
    ADD_PROPERTY_TO_LOCALS(bpc),
    ADD_PROPERTY_TO_LOCALS(wpc),
    ADD_PROPERTY_TO_LOCALS(raw_gma),
    ADD_PROPERTY_TO_LOCALS(lenc),
};
static const mp_rom_map_elem_t camera_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_camera) },
    { MP_ROM_QSTR(MP_QSTR_Camera),    MP_ROM_PTR(&camera_type) },
};

//Helper methods
static void mp_camera_hal_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->initialized) {
        mp_printf(print, "Camera with sensor %s", mp_camera_hal_get_sensor_name(self));
    } else {
        mp_printf(print, "Camera unknown");
    }
}

//API module definition
static MP_DEFINE_CONST_DICT(camera_module_globals, camera_module_globals_table);
static MP_DEFINE_CONST_DICT(camera_camera_locals_dict, camera_camera_locals_table);
MP_DEFINE_CONST_OBJ_TYPE(
    camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_NONE,
    make_new, camera_make_new_stub,
    print, mp_camera_hal_print,
    locals_dict, &camera_camera_locals_dict
);
const mp_obj_module_t camera_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&camera_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_camera, camera_module);