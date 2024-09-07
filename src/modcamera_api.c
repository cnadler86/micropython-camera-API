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

typedef struct mp_camera_obj_t mp_camera_obj;
const mp_obj_type_t camera_type;

static mp_obj_t espcamera_camera_make_new_stub(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
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
    machine_hw_camera_construct(self,data_pins,
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
        machine_hw_camera_init(self);
        (void)machine_hw_camera_capture(self, 100);
    return MP_OBJ_FROM_PTR(self);
}
static mp_obj_t camera_capture(size_t n_args, const mp_obj_t *args){
    //DONE
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_float_t timeout = n_args < 2 ? MICROPY_FLOAT_CONST(0.25) : mp_obj_get_float(args[1]);
    mp_obj_t img = machine_hw_camera_capture(self, timeout);
    return img;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(camera_capture_obj, 1, 2, camera_capture);

static const mp_rom_map_elem_t espcamera_camera_locals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&camera_capture_obj) },
};
static const mp_rom_map_elem_t camera_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_camera) },
    { MP_ROM_QSTR(MP_QSTR_Camera),    MP_ROM_PTR(&camera_type) },
};
static MP_DEFINE_CONST_DICT(camera_module_globals, camera_module_globals_table);
static MP_DEFINE_CONST_DICT(espcamera_camera_locals_dict, espcamera_camera_locals_table);
MP_DEFINE_CONST_OBJ_TYPE(
    camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_NONE,
    make_new, espcamera_camera_make_new_stub,
    locals_dict, &espcamera_camera_locals_dict
);
const mp_obj_module_t camera_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&camera_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_camera, camera_module);