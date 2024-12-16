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

#include "modcamera.h"
#include "esp_err.h"
#include "esp_log.h"
#include "img_converters.h"
#include "mphalport.h"

#define TAG "MPY_CAMERA"

#if !CONFIG_SPIRAM
#error Camera only works on boards configured with spiram
#endif

// Helper functions
static int map(int value, int fromLow, int fromHigh, int toLow, int toHigh) {
    if (fromHigh == fromLow) {
        mp_raise_ValueError(MP_ERROR_TEXT("fromLow und fromHigh shall not be equal"));
    }
    return (int)((int32_t)(value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow);
}

static inline int get_mapped_jpeg_quality(int8_t quality) {
    return map(quality, 0, 100, 63, 0);
}

static inline void check_init(mp_camera_obj_t *self) {
    if (!self->initialized) {
        mp_raise_OSError(ENOENT);
    }
}

static void set_check_xclk_freq(mp_camera_obj_t *self, int32_t xclk_freq_hz) {
    if ( xclk_freq_hz > 20000000) {
        mp_raise_ValueError(MP_ERROR_TEXT("xclk frequency cannot be grather than 20MHz"));
    } else {
        self->camera_config.xclk_freq_hz = xclk_freq_hz;
    }
}

static void set_check_fb_count(mp_camera_obj_t *self, mp_int_t fb_count) {
    if (fb_count > 2) {
        self->camera_config.fb_count = 2;
        mp_warning(NULL, "Frame buffer size limited to 2");
    } else if (fb_count < 1) {
        self->camera_config.fb_count = 1;
        mp_warning(NULL, "Frame buffer size must be >0. Setting it to 1");
    }
    else {
        self->camera_config.fb_count = fb_count;
    }
}

static void set_check_grab_mode(mp_camera_obj_t *self, mp_camera_grabmode_t grab_mode) {
        if (grab_mode != CAMERA_GRAB_WHEN_EMPTY && grab_mode != CAMERA_GRAB_LATEST) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid grab_mode"));
    } else {
        self->camera_config.grab_mode = grab_mode;
    }
}

static void set_check_pixel_format(mp_camera_obj_t *self, mp_camera_pixformat_t pixel_format) {
    if ( pixel_format > PIXFORMAT_RGB555) { //Maximal enum value, but validation should be better since wrong pixelformat leads to reboot.
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid pixel_format"));
    } else {
        self->camera_config.pixel_format = pixel_format;
    }
}

static bool init_camera(mp_camera_obj_t *self) {
    // Correct the quality before it is passed to esp32 driver and then "undo" the correction in the camera_config
    int8_t api_jpeg_quality = self->camera_config.jpeg_quality;
    self->camera_config.jpeg_quality = get_mapped_jpeg_quality(api_jpeg_quality);
    esp_err_t err = esp_camera_init(&self->camera_config);
    self->camera_config.jpeg_quality = api_jpeg_quality;
    check_esp_err_(err);
    return true;
}

// Camera HAL Funcitons
void mp_camera_hal_construct(
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
    mp_camera_grabmode_t grab_mode) {
        // configure camera based on arguments
        self->camera_config.pin_d0 = data_pins[0];
        self->camera_config.pin_d1 = data_pins[1];
        self->camera_config.pin_d2 = data_pins[2];
        self->camera_config.pin_d3 = data_pins[3];
        self->camera_config.pin_d4 = data_pins[4];
        self->camera_config.pin_d5 = data_pins[5];
        self->camera_config.pin_d6 = data_pins[6];
        self->camera_config.pin_d7 = data_pins[7];
        self->camera_config.pin_vsync = vsync_pin;
        self->camera_config.pin_href = href_pin;
        self->camera_config.pin_pclk = pixel_clock_pin;
        self->camera_config.pin_pwdn = powerdown_pin;
        self->camera_config.pin_reset = reset_pin;
        self->camera_config.pin_xclk = external_clock_pin;
        self->camera_config.pin_sscb_sda = sccb_sda_pin;
        self->camera_config.pin_sscb_scl = sccb_scl_pin;

        self->camera_config.frame_size = frame_size;        
        self->camera_config.jpeg_quality = jpeg_quality;    //save value in here, but will be corrected (with map) before passing it to the esp32-driver

        set_check_pixel_format(self, pixel_format);
        set_check_xclk_freq(self, xclk_freq_hz);
        set_check_fb_count(self, fb_count);
        set_check_grab_mode(self, grab_mode);

        // defaul parameters
        self->camera_config.fb_location = CAMERA_FB_IN_PSRAM;
        self->camera_config.ledc_timer = LEDC_TIMER_3;
        self->camera_config.ledc_channel = LEDC_CHANNEL_0;

        self->initialized = false;
        self->captured_buffer = NULL;
        self->converted_buffer.len = 0;
        self->converted_buffer.buf = NULL;
        self->converted_buffer.typecode = 'B';
        self->bmp_out = false;
    }

void mp_camera_hal_init(mp_camera_obj_t *self) {
    if (self->initialized) {
        return;
    }
    #ifndef CONFIG_IDF_TARGET_ESP32S3
        if (self->camera_config.fb_count > 1 && self->camera_config.pixel_format != PIXFORMAT_JPEG) {
            mp_warning(NULL, "It is recomended to use a frame buffer size of 1 for non-JPEG pixel format");
        }
    #endif
    ESP_LOGI(TAG, "Initializing camera");
    self->initialized = init_camera(self);
    ESP_LOGI(TAG, "Camera initialized successfully");
}

void mp_camera_hal_deinit(mp_camera_obj_t *self) {
    if (self->initialized) {
        if (self->converted_buffer.buf) {
            free(self->converted_buffer.buf);
            self->converted_buffer.buf = NULL;
            self->converted_buffer.len = 0;
        }
        if (self->captured_buffer) {
            esp_camera_return_all();
            self->captured_buffer = NULL;
        }
        esp_err_t err = esp_camera_deinit();
        check_esp_err_(err);
        self->initialized = false;
        ESP_LOGI(TAG, "Camera deinitialized");
    }
}

void mp_camera_hal_reconfigure(mp_camera_obj_t *self, mp_camera_framesize_t frame_size, mp_camera_pixformat_t pixel_format, mp_camera_grabmode_t grab_mode, mp_int_t fb_count) {
    check_init(self);
    ESP_LOGI(TAG, "Reconfiguring camera with frame size: %d, pixel format: %d, grab mode: %d, fb count: %d", (int)frame_size, (int)pixel_format, (int)grab_mode, (int)fb_count);
    
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    if (frame_size > sensor_info->max_size) {
        mp_warning(NULL, "Frame size will be scaled down to maximal frame size supported by the camera sensor");
        self->camera_config.frame_size = sensor_info->max_size;
    } else {
        self->camera_config.frame_size = frame_size;
    }

    set_check_pixel_format(self, pixel_format);
    set_check_grab_mode(self, grab_mode);
    set_check_fb_count(self, fb_count);

    check_esp_err_(esp_camera_deinit());
    self->initialized = false;
    self->initialized = init_camera(self);
    ESP_LOGI(TAG, "Camera reconfigured successfully");
}

static bool mp_camera_convert(mp_obj_t self_in, mp_camera_pixformat_t out_format) {
    mp_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ESP_LOGI(TAG, "Converting image to pixel format: %d", out_format);

    if (self->converted_buffer.len > 0 || self->converted_buffer.buf) {
        free(self->converted_buffer.buf);
        self->converted_buffer.len = 0;
        self->converted_buffer.buf = NULL;
    }

    switch (out_format) {
        case PIXFORMAT_JPEG:
            return frame2jpg(self->captured_buffer, self->camera_config.jpeg_quality, self->converted_buffer.buf, &self->converted_buffer.len);

        case PIXFORMAT_RGB888:
            self->converted_buffer.len = self->captured_buffer->width * self->captured_buffer->height * 3;
            self->converted_buffer.buf = (uint8_t *)malloc(self->converted_buffer.len);
            if (!self->converted_buffer.buf) {
                ESP_LOGE(TAG, "converted_buffer malloc failed");
                return false;
            }
            return fmt2rgb888(self->captured_buffer->buf, self->captured_buffer->len, self->captured_buffer->format, self->converted_buffer.buf);

        case PIXFORMAT_RGB565:
            if (self->camera_config.pixel_format == PIXFORMAT_JPEG) {
                self->converted_buffer.len = self->captured_buffer->width * self->captured_buffer->height * 2;
                self->converted_buffer.buf = (uint8_t *)malloc(self->converted_buffer.len);
                if (!self->converted_buffer.buf) {
                    ESP_LOGE(TAG, "converted_buffer malloc failed");
                    return false;
                }
                return jpg2rgb565(self->captured_buffer->buf, self->captured_buffer->len, self->converted_buffer.buf, JPG_SCALE_NONE);
            } else {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Can only convert JPEG to RGB565"));
            }

        default:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Unsupported pixel format for conversion"));
    }
}
mp_obj_t mp_camera_hal_convert(mp_camera_obj_t *self, int8_t out_format) {
    check_init(self);
    if (mp_camera_convert(self, (mp_camera_pixformat_t)out_format)) {
        return mp_obj_new_memoryview('b', self->converted_buffer.len, self->converted_buffer.buf);    
    } else {
        return mp_const_none;
    }
}

mp_obj_t mp_camera_hal_capture(mp_camera_obj_t *self, int8_t out_format) {
    check_init(self);
    if (self->captured_buffer) {
        esp_camera_fb_return(self->captured_buffer);
        self->captured_buffer = NULL;
    }
    
    ESP_LOGI(TAG, "Capturing image");
    self->captured_buffer = esp_camera_fb_get();
    if (!self->captured_buffer) {
        ESP_LOGE(TAG, "Failed to capture image");
        return mp_const_none;
    }
    
    if (out_format >= 0 && (mp_camera_pixformat_t)out_format != self->camera_config.pixel_format) {
        if (mp_camera_convert(self, (mp_camera_pixformat_t)out_format)) {
            esp_camera_fb_return(self->captured_buffer);
            mp_obj_t result = mp_obj_new_memoryview('b', self->converted_buffer.len, self->converted_buffer.buf);
            return result;
        } else {
            return mp_const_none;
        }
    }

    if (self->bmp_out == false) {
        ESP_LOGI(TAG, "Returning image without conversion");
        return mp_obj_new_memoryview('b', self->captured_buffer->len, self->captured_buffer->buf);
    } else {
        ESP_LOGI(TAG, "Returning image as bitmap");
        if (frame2bmp(self->captured_buffer, self->converted_buffer.buf, &self->converted_buffer.len)) {
            esp_camera_fb_return(self->captured_buffer);
            mp_obj_t result = mp_obj_new_memoryview('b', self->converted_buffer.len, self->converted_buffer.buf);
            return result;
        } else {
            free(self->converted_buffer.buf);
            self->converted_buffer.buf = NULL;
            self->converted_buffer.len = 0;
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to convert image to BMP"));
            return mp_const_none;
        }
    }
} // mp_camera_hal_capture

bool mp_camera_hal_initialized(mp_camera_obj_t *self){
    return self->initialized;
}

const mp_rom_map_elem_t mp_camera_hal_pixel_format_table[] = {
    { MP_ROM_QSTR(MP_QSTR_JPEG),            MP_ROM_INT(PIXFORMAT_JPEG) },
    { MP_ROM_QSTR(MP_QSTR_YUV422),          MP_ROM_INT(PIXFORMAT_YUV422) },
    { MP_ROM_QSTR(MP_QSTR_GRAYSCALE),       MP_ROM_INT(PIXFORMAT_GRAYSCALE) },
    { MP_ROM_QSTR(MP_QSTR_RGB565),          MP_ROM_INT(PIXFORMAT_RGB565) },
    { MP_ROM_QSTR(MP_QSTR_RGB888),          MP_ROM_INT(PIXFORMAT_RGB888) },
};

const mp_rom_map_elem_t mp_camera_hal_frame_size_table[] = {
    { MP_ROM_QSTR(MP_QSTR_R96X96),    MP_ROM_INT(FRAMESIZE_96X96) },
    { MP_ROM_QSTR(MP_QSTR_QQVGA),     MP_ROM_INT(FRAMESIZE_QQVGA) },
    { MP_ROM_QSTR(MP_QSTR_R128x128),  MP_ROM_INT(FRAMESIZE_128X128) },
    { MP_ROM_QSTR(MP_QSTR_QCIF),      MP_ROM_INT(FRAMESIZE_QCIF) },
    { MP_ROM_QSTR(MP_QSTR_HQVGA),     MP_ROM_INT(FRAMESIZE_HQVGA) },
    { MP_ROM_QSTR(MP_QSTR_R240X240),  MP_ROM_INT(FRAMESIZE_240X240) },
    { MP_ROM_QSTR(MP_QSTR_QVGA),      MP_ROM_INT(FRAMESIZE_QVGA) },
    { MP_ROM_QSTR(MP_QSTR_R320X320),  MP_ROM_INT(FRAMESIZE_320X320) },
    { MP_ROM_QSTR(MP_QSTR_CIF),       MP_ROM_INT(FRAMESIZE_CIF) },
    { MP_ROM_QSTR(MP_QSTR_HVGA),      MP_ROM_INT(FRAMESIZE_HVGA) },
    { MP_ROM_QSTR(MP_QSTR_VGA),       MP_ROM_INT(FRAMESIZE_VGA) },
    { MP_ROM_QSTR(MP_QSTR_SVGA),      MP_ROM_INT(FRAMESIZE_SVGA) },
    { MP_ROM_QSTR(MP_QSTR_XGA),       MP_ROM_INT(FRAMESIZE_XGA) },
    { MP_ROM_QSTR(MP_QSTR_HD),        MP_ROM_INT(FRAMESIZE_HD) },
    { MP_ROM_QSTR(MP_QSTR_SXGA),      MP_ROM_INT(FRAMESIZE_SXGA) },
    { MP_ROM_QSTR(MP_QSTR_UXGA),      MP_ROM_INT(FRAMESIZE_UXGA) },
    { MP_ROM_QSTR(MP_QSTR_FHD),       MP_ROM_INT(FRAMESIZE_FHD) },
    { MP_ROM_QSTR(MP_QSTR_P_HD),      MP_ROM_INT(FRAMESIZE_P_HD) },
    { MP_ROM_QSTR(MP_QSTR_P_3MP),     MP_ROM_INT(FRAMESIZE_P_3MP) },
    { MP_ROM_QSTR(MP_QSTR_QXGA),      MP_ROM_INT(FRAMESIZE_QXGA) },
    { MP_ROM_QSTR(MP_QSTR_QHD),       MP_ROM_INT(FRAMESIZE_QHD) },
    { MP_ROM_QSTR(MP_QSTR_WQXGA),     MP_ROM_INT(FRAMESIZE_WQXGA) },
    { MP_ROM_QSTR(MP_QSTR_P_FHD),     MP_ROM_INT(FRAMESIZE_P_FHD) },
    { MP_ROM_QSTR(MP_QSTR_QSXGA),     MP_ROM_INT(FRAMESIZE_QSXGA) },
};

const mp_rom_map_elem_t mp_camera_hal_grab_mode_table[] = {
    { MP_ROM_QSTR(MP_QSTR_WHEN_EMPTY), MP_ROM_INT(CAMERA_GRAB_WHEN_EMPTY) },
    { MP_ROM_QSTR(MP_QSTR_LATEST),     MP_ROM_INT(CAMERA_GRAB_LATEST) },
};

const mp_rom_map_elem_t mp_camera_hal_gainceiling_table[] = {
    { MP_ROM_QSTR(MP_QSTR_2X),      MP_ROM_INT(GAINCEILING_2X) },
    { MP_ROM_QSTR(MP_QSTR_4X),      MP_ROM_INT(GAINCEILING_4X) },
    { MP_ROM_QSTR(MP_QSTR_8X),      MP_ROM_INT(GAINCEILING_8X) },
    { MP_ROM_QSTR(MP_QSTR_16X),     MP_ROM_INT(GAINCEILING_16X) },
    { MP_ROM_QSTR(MP_QSTR_32X),     MP_ROM_INT(GAINCEILING_32X) },
    { MP_ROM_QSTR(MP_QSTR_64X),     MP_ROM_INT(GAINCEILING_64X) },
    { MP_ROM_QSTR(MP_QSTR_128X),    MP_ROM_INT(GAINCEILING_128X) },
};

//TODO: Makros with convertion function, since the API will use standarized values.
// Helper functions to get and set camera and sensor information
#define SENSOR_STATUS_GETSET_IN_RANGE(type, name, status_field_name, setter_function_name, min_val, max_val) \
    SENSOR_GETSET_IN_RANGE(type, name, status.status_field_name, setter_function_name, min_val, max_val)

#define SENSOR_STATUS_GETSET(type, name, status_field_name, setter_function_name) \
    SENSOR_GETSET(type, name, status.status_field_name, setter_function_name)

// For subsequent modules using this as example, you will probably only need the makros below.
#define SENSOR_GETSET(type, name, field_name, setter_function_name) \
    SENSOR_GET(type, name, field_name) \
    SENSOR_SET(type, name, setter_function_name)

#define SENSOR_GETSET_IN_RANGE(type, name, field_name, setter_function_name, min_val, max_val) \
    SENSOR_GET(type, name, field_name) \
    SENSOR_SET_IN_RANGE(type, name, setter_function_name, min_val, max_val)

#define SENSOR_GET(type, name, status_field_name) \
    type mp_camera_hal_get_##name(mp_camera_obj_t * self) { \
        check_init(self); \
        sensor_t *sensor = esp_camera_sensor_get(); \
        return sensor->status_field_name; \
    }

#define SENSOR_SET(type, name, setter_function_name) \
    void mp_camera_hal_set_##name(mp_camera_obj_t * self, type value) { \
        check_init(self); \
        sensor_t *sensor = esp_camera_sensor_get(); \
        if (!sensor->setter_function_name) { \
            mp_raise_ValueError(MP_ERROR_TEXT("No attribute " #name)); \
        } \
        if (sensor->setter_function_name(sensor, value) < 0) { \
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid setting for " #name)); \
        } \
    }

#define SENSOR_SET_IN_RANGE(type, name, setter_function_name, min_val, max_val) \
    void mp_camera_hal_set_##name(mp_camera_obj_t * self, type value) { \
        sensor_t *sensor = esp_camera_sensor_get(); \
        check_init(self); \
        if (value < min_val || value > max_val) { \
            mp_raise_ValueError(MP_ERROR_TEXT(#name " value must be between " #min_val " and " #max_val)); \
        } \
        if (!sensor->setter_function_name) { \
            mp_raise_ValueError(MP_ERROR_TEXT("No attribute " #name)); \
        } \
        if (sensor->setter_function_name(sensor, value) < 0) { \
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid setting for " #name)); \
        } \
    }

SENSOR_GET(framesize_t, frame_size, status.framesize);
SENSOR_STATUS_GETSET_IN_RANGE(int, contrast, contrast, set_contrast, -2, 2);
SENSOR_STATUS_GETSET_IN_RANGE(int, brightness, brightness, set_brightness, -2, 2);
SENSOR_STATUS_GETSET_IN_RANGE(int, saturation, saturation, set_saturation, -2, 2);
SENSOR_STATUS_GETSET_IN_RANGE(int, sharpness, sharpness, set_sharpness, -2, 2);
SENSOR_STATUS_GETSET(int, denoise, denoise, set_denoise);
SENSOR_STATUS_GETSET(mp_camera_gainceiling_t, gainceiling, gainceiling, set_gainceiling);
SENSOR_STATUS_GETSET(bool, colorbar, colorbar, set_colorbar);
SENSOR_STATUS_GETSET(bool, whitebal, awb, set_whitebal);
SENSOR_STATUS_GETSET(bool, gain_ctrl, agc, set_gain_ctrl);
SENSOR_STATUS_GETSET(bool, exposure_ctrl, aec, set_exposure_ctrl);
SENSOR_STATUS_GETSET(bool, hmirror, hmirror, set_hmirror);
SENSOR_STATUS_GETSET(bool, vflip, vflip, set_vflip);
SENSOR_STATUS_GETSET(bool, aec2, aec2, set_aec2);
SENSOR_STATUS_GETSET(bool, awb_gain, awb_gain, set_awb_gain);
SENSOR_STATUS_GETSET(int, agc_gain, agc_gain, set_agc_gain); //in_Range not needed since driver limits value
SENSOR_STATUS_GETSET(int, aec_value, aec_value, set_aec_value); //in_Range not needed since driver limits value
SENSOR_STATUS_GETSET_IN_RANGE(int, special_effect, special_effect, set_special_effect, 0, 6);
SENSOR_STATUS_GETSET_IN_RANGE(int, wb_mode, wb_mode, set_wb_mode, 0, 4);
SENSOR_STATUS_GETSET_IN_RANGE(int, ae_level, ae_level, set_ae_level, -2, 2);
SENSOR_STATUS_GETSET(bool, dcw, dcw, set_dcw);
SENSOR_STATUS_GETSET(bool, bpc, bpc, set_bpc);
SENSOR_STATUS_GETSET(bool, wpc, wpc, set_wpc);
SENSOR_STATUS_GETSET(bool, raw_gma, raw_gma, set_raw_gma);
SENSOR_STATUS_GETSET(bool, lenc, lenc, set_lenc);

void mp_camera_hal_set_frame_size(mp_camera_obj_t * self, framesize_t value) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor->set_framesize) {
        mp_raise_ValueError(MP_ERROR_TEXT("No attribute frame_size"));
    }

    if (self->captured_buffer) {
        esp_camera_return_all();
        self->captured_buffer = NULL;
    }

    if (sensor->set_framesize(sensor, value) < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid setting for frame_size"));
    } else {
        self->camera_config.frame_size = value;
    }
}

int mp_camera_hal_get_quality(mp_camera_obj_t * self) {
    check_init(self);
    return self->camera_config.jpeg_quality;
}

void mp_camera_hal_set_quality(mp_camera_obj_t * self, int value) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor->set_quality) {
        mp_raise_ValueError(MP_ERROR_TEXT("No attribute quality"));
    }
    if (sensor->set_quality(sensor, get_mapped_jpeg_quality(value)) < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid setting for quality"));
    } else {
        self->camera_config.jpeg_quality = value;
    }
}

mp_camera_pixformat_t mp_camera_hal_get_pixel_format(mp_camera_obj_t *self) {
    return self->camera_config.pixel_format;
}

camera_grab_mode_t mp_camera_hal_get_grab_mode(mp_camera_obj_t *self) {
    return self->camera_config.grab_mode;
}

int mp_camera_hal_get_fb_count(mp_camera_obj_t *self) {
    return self->camera_config.fb_count;
}

const char *mp_camera_hal_get_sensor_name(mp_camera_obj_t *self) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->name;
}

bool mp_camera_hal_get_supports_jpeg(mp_camera_obj_t *self) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->support_jpeg;
}

mp_camera_framesize_t mp_camera_hal_get_max_frame_size(mp_camera_obj_t *self) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->max_size;
}

int mp_camera_hal_get_address(mp_camera_obj_t *self) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->sccb_addr;
}

int mp_camera_hal_get_pixel_width(mp_camera_obj_t *self) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    framesize_t framesize = sensor->status.framesize;
    return resolution[framesize].width;
}

int mp_camera_hal_get_pixel_height(mp_camera_obj_t *self) {
    check_init(self);
    sensor_t *sensor = esp_camera_sensor_get();
    framesize_t framesize = sensor->status.framesize;
    return resolution[framesize].height;
}
