#include "py/obj.h"
#include "py/runtime.h"
#include "img_converters.h"

// Wrapper für frame2jpg
static mp_obj_t mp_frame2jpg(mp_obj_t fb_in, mp_obj_t quality_in) {
    if (mp_obj_get_type(fb_in) != &mp_type_memoryview) {
        mp_raise_TypeError("Expected a memoryview object");
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(fb_in, &bufinfo, MP_BUFFER_READ);
    camera_fb_t *fb = (camera_fb_t *)bufinfo.buf;

    uint8_t quality = mp_obj_get_int(quality_in);

    uint8_t *out = NULL;
    size_t out_len = 0;

    bool success = frame2jpg(fb, quality, &out, &out_len);

    if (!success) {
        mp_raise_msg(&mp_type_RuntimeError, "JPEG conversion failed");
    }

    mp_obj_t result = mp_obj_new_bytearray(out_len, out);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mp_frame2jpg_obj, mp_frame2jpg);

// Wrapper für frame2bmp
static mp_obj_t mp_frame2bmp(mp_obj_t fb_in) {
    if (mp_obj_get_type(fb_in) != &mp_type_memoryview) {
        mp_raise_TypeError("Expected a memoryview object");
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(fb_in, &bufinfo, MP_BUFFER_READ);
    camera_fb_t *fb = (camera_fb_t *)bufinfo.buf;

    uint8_t *out = NULL;
    size_t out_len = 0;

    bool success = frame2bmp(fb, &out, &out_len);

    if (!success) {
        mp_raise_msg(&mp_type_RuntimeError, "BMP conversion failed");
    }

    mp_obj_t result = mp_obj_new_bytearray(out_len, out);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_frame2bmp_obj, mp_frame2bmp);

// Wrapper für fmt2rgb888
static mp_obj_t mp_fmt2rgb888(mp_obj_t src_buf_in, mp_obj_t format_in) {
    if (mp_obj_get_type(src_buf_in) != &mp_type_memoryview) {
        mp_raise_TypeError("Expected a memoryview object");
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(src_buf_in, &bufinfo, MP_BUFFER_READ);
    const uint8_t *src_buf = (const uint8_t *)bufinfo.buf;
    size_t src_len = bufinfo.len;

    pixformat_t format = mp_obj_get_int(format_in);

    uint8_t *rgb_buf = m_new(uint8_t, bufinfo.len * 3); // Allocate buffer for RGB888

    bool success = fmt2rgb888(src_buf, src_len, format, rgb_buf);

    if (!success) {
        mp_raise_msg(&mp_type_RuntimeError, "RGB888 conversion failed");
    }

    mp_obj_t result = mp_obj_new_bytearray(src_len * 3, rgb_buf);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mp_fmt2rgb888_obj, mp_fmt2rgb888);

// Modulmethoden
static const mp_rom_map_elem_t img_converters_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_frame2jpg), MP_ROM_PTR(&mp_frame2jpg_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame2bmp), MP_ROM_PTR(&mp_frame2bmp_obj) },
    { MP_ROM_QSTR(MP_QSTR_fmt2rgb888), MP_ROM_PTR(&mp_fmt2rgb888_obj) },
};

static MP_DEFINE_CONST_DICT(img_converters_module_globals, img_converters_module_globals_table);

// Moduldefinition
const mp_obj_module_t img_converters_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&img_converters_module_globals,
};

// Modulregistrierung
MP_REGISTER_MODULE(MP_QSTR_img_converters, img_converters_module);