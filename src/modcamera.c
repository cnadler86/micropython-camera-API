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

#define TAG "ESP32_MPY_CAMERA"

#if !CONFIG_SPIRAM
#error Camera only works on boards configured with spiram
#endif

#if MP_TASK_COREID == 0
    #define CONFIG_CAMERA_CORE0 1
#elif MP_TASK_COREID == 1
    #define CONFIG_CAMERA_CORE1 1
#endif

// Supporting functions
static void raise_micropython_error_from_esp_err(esp_err_t err) {
    switch (err) {
        case ESP_OK:
            return;

        case ESP_ERR_NO_MEM:
            mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Out of memory"));
            break;

        case ESP_ERR_INVALID_ARG:
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid argument"));
            break;

        case ESP_ERR_INVALID_STATE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Invalid state"));
            break;

        case ESP_ERR_NOT_FOUND:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Camera not found"));
            break;

        case ESP_ERR_NOT_SUPPORTED:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Operation/Function not supported/implemented"));
            break;

        case ESP_ERR_TIMEOUT:
            mp_raise_OSError(MP_ETIMEDOUT);
            break;

        default:
            // mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Unknown error 0x%04x"), err);
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Unknown error"));
            break;
    }
}

static int map(int value, int fromLow, int fromHigh, int toLow, int toHigh) {
    if (fromHigh == fromLow) {
        mp_raise_ValueError(MP_ERROR_TEXT("fromLow und fromHigh shall not be equal"));
    }
    return (int)((int32_t)(value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow);
}

static inline int get_mapped_jpeg_quality(int8_t quality) {
    return map(quality, 0, 100, 63, 0);
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
        self->camera_config.pixel_format = pixel_format;
        self->camera_config.frame_size = frame_size;        
        // self->camera_config.jpeg_quality = (int8_t)map(jpeg_quality,0,100,63,0);    //0-63 lower number means higher quality.
        self->camera_config.jpeg_quality = jpeg_quality;    //save value in here, but will be corrected (with map) before passing it to the esp32-driver
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
        if ( xclk_freq_hz > 20000000) {
            mp_raise_ValueError(MP_ERROR_TEXT("xclk frequency cannot be grather than 20MHz"));
        } else {
            self->camera_config.xclk_freq_hz = xclk_freq_hz;
        }

        if (fb_count > 2) {
            self->camera_config.fb_count = 2;
            mp_warning(NULL, "Frame buffer size limited to 2");
        } else if (fb_count < 1) {
            self->camera_config.fb_count = 1;
            mp_warning(NULL, "Frame buffer size must be >0. Setting it to 1");
        }
        else {
            self->camera_config.fb_count = fb_count;          //if more than one, i2s runs in continuous mode. TODO: Test with others than JPEG
        }
        self->camera_config.grab_mode = grab_mode;

        // defaul parameters
        self->camera_config.fb_location = CAMERA_FB_IN_PSRAM;
        self->camera_config.ledc_timer = LEDC_TIMER_0;
        self->camera_config.ledc_channel = LEDC_CHANNEL_0;

        self->initialized = false;
        self->captured_buffer = NULL;
    }

void mp_camera_hal_init(mp_camera_obj_t *self) {
    if (self->initialized) {
        return;
    }
    ESP_LOGI(TAG, "Initializing camera");
    camera_config_t temp_config = self->camera_config;
    temp_config.frame_size = FRAMESIZE_QVGA;        //use values supported by all cameras
    temp_config.pixel_format = PIXFORMAT_RGB565;    //use values supported by all cameras
    temp_config.jpeg_quality = get_mapped_jpeg_quality(self->camera_config.jpeg_quality);
    esp_err_t err = esp_camera_init(&temp_config);
    if (err != ESP_OK) {
        self->initialized = false;
        raise_micropython_error_from_esp_err(err);
    } else {
        self->initialized = true;
    }
    mp_camera_hal_reconfigure(self, self->camera_config.frame_size, self->camera_config.pixel_format, 
        self->camera_config.grab_mode, self->camera_config.fb_count);
    ESP_LOGI(TAG, "Camera initialized successfully");
}

void mp_camera_hal_deinit(mp_camera_obj_t *self) {
    if (self->initialized) {
        if (self->captured_buffer) {
            esp_camera_fb_return(self->captured_buffer);
            self->captured_buffer = NULL;
        }
        esp_err_t err = esp_camera_deinit();
        raise_micropython_error_from_esp_err(err);
        self->initialized = false;
        ESP_LOGI(TAG, "Camera deinitialized");
    }
}

void mp_camera_hal_reconfigure(mp_camera_obj_t *self, mp_camera_framesize_t frame_size, mp_camera_pixformat_t pixel_format, mp_camera_grabmode_t grab_mode, mp_int_t fb_count) {
    if (self->initialized) {
        ESP_LOGI(TAG, "Reconfiguring camera");
        sensor_t *sensor = esp_camera_sensor_get();
        camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);

        if (PIXFORMAT_JPEG == self->camera_config.pixel_format && (!sensor_info->support_jpeg)) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Sensor does not support JPEG"));
        }

        if (frame_size > sensor_info->max_size) {
            mp_warning(NULL, "Frame size will be scaled down to maximal frame size supported by the camera sensor");
            self->camera_config.frame_size = sensor_info->max_size;
        } else {
            self->camera_config.frame_size = frame_size;
        }

        if ( pixel_format > PIXFORMAT_RGB555) { //Maximal enum value, but validation should be better since wrong pixelformat leads to reboot.
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid pixel_format"));
        } else {
            self->camera_config.pixel_format = pixel_format;
        }
        
        if (grab_mode != CAMERA_GRAB_WHEN_EMPTY && grab_mode != CAMERA_GRAB_LATEST) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid grab_mode"));
        } else {
            self->camera_config.grab_mode = grab_mode;
        }
        
        if (fb_count > 2) {
            self->camera_config.fb_count = 2;
            mp_warning(NULL, "Frame buffer size limited to 2");
        } else if (fb_count < 1) {
            self->camera_config.fb_count = 1;
            mp_warning(NULL, "Set to min frame buffer size of 1");
        }
        else {
            self->camera_config.fb_count = fb_count;
        }
        
        raise_micropython_error_from_esp_err(esp_camera_deinit());

        // Correct the quality before it is passed to esp32 driver and then "undo" the correction in the camera_config
        int8_t api_jpeg_quality = self->camera_config.jpeg_quality;
        self->camera_config.jpeg_quality = get_mapped_jpeg_quality(api_jpeg_quality);
        esp_err_t err = esp_camera_init(&self->camera_config);
        self->camera_config.jpeg_quality = api_jpeg_quality;

        if (err != ESP_OK) {
            self->initialized = false;
            raise_micropython_error_from_esp_err(err);
        } else {
            ESP_LOGI(TAG, "Camera reconfigured successfully");
        }
    }
}

mp_obj_t mp_camera_hal_capture(mp_camera_obj_t *self, int8_t out_format) {
    if (!self->initialized) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to capture image: Camera not initialized"));
    }
    if (self->captured_buffer) {
        esp_camera_fb_return(self->captured_buffer);
        self->captured_buffer = NULL;
    }
    
    static size_t out_len = 0;
    static uint8_t *out_buf = NULL;
    if (out_len>0 || out_buf) {
        free(out_buf);
        out_len = 0;
        out_buf = NULL;
    }

    ESP_LOGI(TAG, "Capturing image");
    self->captured_buffer = esp_camera_fb_get();
    if (!self->captured_buffer) {
        ESP_LOGE(TAG, "Failed to capture image");
        return mp_const_none;
    }
    
    if (out_format >= 0 && (mp_camera_pixformat_t)out_format != self->camera_config.pixel_format) {
        switch (out_format) {
            case PIXFORMAT_JPEG:
                if (frame2jpg(self->captured_buffer, self->camera_config.jpeg_quality, &out_buf, &out_len)) {
                    esp_camera_fb_return(self->captured_buffer);
                    mp_obj_t result = mp_obj_new_memoryview('b', out_len, out_buf);
                    return result;
                } else {
                    return mp_const_none;
                }

            case PIXFORMAT_RGB888:
                out_len = self->captured_buffer->width * self->captured_buffer->height * 3;
                out_buf = (uint8_t *)malloc(out_len);
                if (!out_buf) {
                    ESP_LOGE(TAG, "out_buf malloc failed");
                    return mp_const_none;
                }
                if (fmt2rgb888(self->captured_buffer->buf, self->captured_buffer->len, self->captured_buffer->format, out_buf)) {
                    esp_camera_fb_return(self->captured_buffer);
                    mp_obj_t result = mp_obj_new_memoryview('b', out_len, out_buf);
                    return result;
                } else {
                    return mp_const_none;
                }

            case PIXFORMAT_RGB565:
                if(self->camera_config.pixel_format == PIXFORMAT_JPEG){
                    if (jpg2rgb565(self->captured_buffer->buf, self->captured_buffer->len, out_buf, JPG_SCALE_NONE)) {
                        esp_camera_fb_return(self->captured_buffer);
                        mp_obj_t result = mp_obj_new_memoryview('b', out_len, out_buf);
                        return result;
                    } else {
                        return mp_const_none;
                    }
                } else {
                    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Can only convert JPEG to RGB565"));
                    return mp_const_none;
                }

            default:
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Unsupported pixel format for conversion"));
                return mp_const_none;

        }
    }

    if (self->bmp_out == false) {
        ESP_LOGI(TAG, "Returning imgae without conversion");
        return mp_obj_new_memoryview('b', self->captured_buffer->len, self->captured_buffer->buf);
    } else {
        ESP_LOGI(TAG, "Returning image as bitmap");
        if (frame2bmp(self->captured_buffer, &out_buf, &out_len)) {
            esp_camera_fb_return(self->captured_buffer);
            mp_obj_t result = mp_obj_new_memoryview('b', out_len, out_buf);
            return result;
        } else {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Failed to convert image to BMP"));
            return mp_const_none;
        }
    }
}

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
    { MP_ROM_QSTR(MP_QSTR_QCIF),      MP_ROM_INT(FRAMESIZE_QCIF) },
    { MP_ROM_QSTR(MP_QSTR_HQVGA),     MP_ROM_INT(FRAMESIZE_HQVGA) },
    { MP_ROM_QSTR(MP_QSTR_R240X240),  MP_ROM_INT(FRAMESIZE_240X240) },
    { MP_ROM_QSTR(MP_QSTR_QVGA),      MP_ROM_INT(FRAMESIZE_QVGA) },
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
        if (!self->initialized) { \
            mp_raise_ValueError(MP_ERROR_TEXT("Camera not initialized")); \
        } \
        sensor_t *sensor = esp_camera_sensor_get(); \
        return sensor->status_field_name; \
    }

#define SENSOR_SET(type, name, setter_function_name) \
    void mp_camera_hal_set_##name(mp_camera_obj_t * self, type value) { \
        if (!self->initialized) { \
            mp_raise_ValueError(MP_ERROR_TEXT("Camera not initialized")); \
        } \
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
        if (!self->initialized) { \
            mp_raise_ValueError(MP_ERROR_TEXT("Camera not initialized")); \
        } \
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
    if (!self->initialized) {
        mp_raise_ValueError(MP_ERROR_TEXT("Camera not initialized"));
    }

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
    if (!self->initialized) {
        mp_raise_ValueError(MP_ERROR_TEXT("Camera not initialized"));
    }
    return self->camera_config.jpeg_quality;
}

void mp_camera_hal_set_quality(mp_camera_obj_t * self, int value) {
    if (!self->initialized) {
        mp_raise_ValueError(MP_ERROR_TEXT("Camera not initialized"));
    }
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
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->name;
}

bool mp_camera_hal_get_supports_jpeg(mp_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->support_jpeg;
}

mp_camera_framesize_t mp_camera_hal_get_max_frame_size(mp_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->max_size;
}

int mp_camera_hal_get_address(mp_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    camera_sensor_info_t *sensor_info = esp_camera_sensor_get_info(&sensor->id);
    return sensor_info->sccb_addr;
}

int mp_camera_hal_get_pixel_width(mp_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    framesize_t framesize = sensor->status.framesize;
    return resolution[framesize].width;
}

int mp_camera_hal_get_pixel_height(mp_camera_obj_t *self) {
    sensor_t *sensor = esp_camera_sensor_get();
    framesize_t framesize = sensor->status.framesize;
    return resolution[framesize].height;
}
