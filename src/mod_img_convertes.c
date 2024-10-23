#include "py/obj.h"
#include "py/runtime.h"
#include "img_converters.h"

// Helper function to get buffer info from memoryview or bytearray
static void get_buffer_info(mp_obj_t obj, mp_buffer_info_t *bufinfo) {
    if (mp_obj_get_type(obj) == &mp_type_memoryview || mp_obj_get_type(obj) == &mp_type_bytearray) {
        mp_get_buffer_raise(obj, bufinfo, MP_BUFFER_READ);
    } else {
        mp_raise_TypeError("Expected a memoryview or bytearray object");
    }
}

// Wrapper für frame2jpg
static mp_obj_t mp_frame2jpg(mp_obj_t fb_in, mp_obj_t quality_in) {
    mp_buffer_info_t bufinfo;
    get_buffer_info(fb_in, &bufinfo);
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
    mp_buffer_info_t bufinfo;
    get_buffer_info(fb_in, &bufinfo);
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

// Wrapper für fmt2rgb888 mit optionalem format-Argument
static mp_obj_t mp_fmt2rgb888(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    get_buffer_info(args[0], &bufinfo);
    const uint8_t *src_buf = (const uint8_t *)bufinfo.buf;
    size_t src_len = bufinfo.len;

    pixformat_t format = PIXFORMAT_JPEG; // Standardformat ist JPEG
    if (n_args > 1) {
        format = mp_obj_get_int(args[1]);
    }

    uint8_t *rgb_buf = m_new(uint8_t, bufinfo.len * 3); // Allocate buffer for RGB888

    bool success = fmt2rgb888(src_buf, src_len, format, rgb_buf);

    if (!success) {
        mp_raise_msg(&mp_type_RuntimeError, "RGB888 conversion failed");
    }

    mp_obj_t result = mp_obj_new_bytearray(src_len * 3, rgb_buf);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_fmt2rgb888_obj, 1, 2, mp_fmt2rgb888);

// Wrapper für jpg2rgb565 mit optionalem scale-Argument
static mp_obj_t mp_jpg2rgb565(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    get_buffer_info(args[0], &bufinfo);
    const uint8_t *src_buf = (const uint8_t *)bufinfo.buf;
    size_t src_len = bufinfo.len;

    jpg_scale_t scale = JPG_SCALE_NONE; // Standardwert ist JPG_SCALE_NONE
    if (n_args > 1) {
        scale = mp_obj_get_int(args[1]);
    }

    uint8_t *rgb_buf = m_new(uint8_t, bufinfo.len * 2); // Allocate buffer for RGB565

    bool success = jpg2rgb565(src_buf, src_len, rgb_buf, scale);

    if (!success) {
        mp_raise_msg(&mp_type_RuntimeError, "RGB565 conversion failed");
    }

    mp_obj_t result = mp_obj_new_bytearray(src_len * 2, rgb_buf);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_jpg2rgb565_obj, 1, 2, mp_jpg2rgb565);

// Modulmethoden
static const mp_rom_map_elem_t img_converters_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_frame2jpg), MP_ROM_PTR(&mp_frame2jpg_obj) },
    { MP_ROM_QSTR(MP_QSTR_frame2bmp), MP_ROM_PTR(&mp_frame2bmp_obj) },
    { MP_ROM_QSTR(MP_QSTR_fmt2rgb888), MP_ROM_PTR(&mp_fmt2rgb888_obj) },
    { MP_ROM_QSTR(MP_QSTR_jpg2rgb565), MP_ROM_PTR(&mp_jpg2rgb565_obj) },
};

static MP_DEFINE_CONST_DICT(img_converters_module_globals, img_converters_module_globals_table);

// Moduldefinition
const mp_obj_module_t img_converters_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&img_converters_module_globals,
};

// Modulregistrierung
MP_REGISTER_MODULE(MP_QSTR_img_converters, img_converters_module);