from __future__ import annotations
from typing import Final

class GainCeiling():
    X2: Final[int] = 0    # 2X gain
    X4: Final[int] = 1    # 4X gain
    X8: Final[int] = 2    # 8X gain
    X16: Final[int] = 3   # 16X gain
    X32: Final[int] = 4   # 32X gain
    X64: Final[int] = 5   # 64X gain
    X128: Final[int] = 6  # 128X gain
    def __init__(self, *argv, **kwargs) -> None:
        ...


class GrabMode():
    WHEN_EMPTY: Final[int] = 0
    LATEST: Final[int] = 1
    def __init__(self, *argv, **kwargs) -> None:
        ...


class PixelFormat():
    RGB555: Final[int] = 8
    RGB565: Final[int] = 0
    RGB888: Final[int] = 5
    YUV420: Final[int] = 2
    YUV422: Final[int] = 1
    RGB444: Final[int] = 7
    GRAYSCALE: Final[int] = 3
    JPEG: Final[int] = 4
    RAW: Final[int] = 6
    def __init__(self, *argv, **kwargs) -> None:
        ...


class FrameSize():
    R240X240: Final[int] = 5
    XGA: Final[int] = 12
    R320X320: Final[int] = 7
    QVGA: Final[int] = 6
    R128x128: int = 2
    QXGA: Final[int] = 19
    VGA: Final[int] = 10
    R96X96: Final[int] = 0
    WQXGA: Final[int] = 21
    SVGA: Final[int] = 11
    UXGA: Final[int] = 15
    SXGA: Final[int] = 14
    HQVGA: Final[int] = 4
    QSXGA: Final[int] = 23
    HVGA: Final[int] = 9
    CIF: Final[int] = 8
    HD: Final[int] = 13
    FHD: Final[int] = 16
    QHD: Final[int] = 20
    P_3MP: Final[int] = 18
    QQVGA: Final[int] = 1
    P_FHD: Final[int] = 22
    QCIF: Final[int] = 3
    P_HD: Final[int] = 17
    def __init__(self, *argv, **kwargs) -> None:
        ...


class Camera():
    def __init__(self, *,
                 data_pins: list[int] | None = None,
                 pclk_pin: int | None = None,
                 vsync_pin: int | None = None,
                 href_pin: int | None = None,
                 sda_pin: int | None = None,
                 scl_pin: int | None = None,
                 xclk_pin: int | None = None,
                 xclk_freq: int = 20000000,
                 powerdown_pin: int = -1,
                 reset_pin: int = -1,
                 i2c: object | None = None,
                 pixel_format: int = PixelFormat.RGB565,
                 frame_size: int = FrameSize.QVGA,
                 jpeg_quality: int = 85,
                 fb_count: int = 1,
                 grab_mode: int = GrabMode.WHEN_EMPTY,
                 init: bool = True) -> None:
        ...

    # Properties (read-only)
    @property
    def pixel_format(self) -> int:
        """Get current pixel format (read-only)."""
        ...

    @property
    def grab_mode(self) -> int:
        """Get current grab mode (read-only)."""
        ...

    @property
    def fb_count(self) -> int:
        """Get frame buffer count (read-only)."""
        ...

    @property
    def pixel_width(self) -> int:
        """Get frame width in pixels (read-only)."""
        ...

    @property
    def pixel_height(self) -> int:
        """Get frame height in pixels (read-only)."""
        ...

    @property
    def max_frame_size(self) -> int:
        """Get maximum supported frame size (read-only)."""
        ...

    @property
    def sensor_name(self) -> str:
        """Get camera sensor name (read-only)."""
        ...

    # Properties (read-write)
    @property
    def frame_size(self) -> int:
        """Get/set frame size."""
        ...

    @frame_size.setter
    def frame_size(self, value: int) -> None:
        ...

    @property
    def contrast(self) -> int:
        """Get/set contrast level (-2 to 2)."""
        ...

    @contrast.setter
    def contrast(self, value: int) -> None:
        ...

    @property
    def brightness(self) -> int:
        """Get/set brightness level (-2 to 2)."""
        ...

    @brightness.setter
    def brightness(self, value: int) -> None:
        ...

    @property
    def saturation(self) -> int:
        """Get/set saturation level (-2 to 2)."""
        ...

    @saturation.setter
    def saturation(self, value: int) -> None:
        ...

    @property
    def sharpness(self) -> int:
        """Get/set sharpness level (-2 to 2)."""
        ...

    @sharpness.setter
    def sharpness(self, value: int) -> None:
        ...

    @property
    def denoise(self) -> int:
        """Get/set denoise level."""
        ...

    @denoise.setter
    def denoise(self, value: int) -> None:
        ...

    @property
    def gainceiling(self) -> int:
        """Get/set gain ceiling."""
        ...

    @gainceiling.setter
    def gainceiling(self, value: int) -> None:
        ...

    @property
    def quality(self) -> int:
        """Get/set JPEG quality (0-63)."""
        ...

    @quality.setter
    def quality(self, value: int) -> None:
        ...

    @property
    def colorbar(self) -> bool:
        """Get/set color bar test pattern."""
        ...

    @colorbar.setter
    def colorbar(self, value: bool) -> None:
        ...

    @property
    def whitebal(self) -> bool:
        """Get/set white balance."""
        ...

    @whitebal.setter
    def whitebal(self, value: bool) -> None:
        ...

    @property
    def gain_ctrl(self) -> bool:
        """Get/set Auto Gain Control."""
        ...

    @gain_ctrl.setter
    def gain_ctrl(self, value: bool) -> None:
        ...

    @property
    def exposure_ctrl(self) -> bool:
        """Get/set Auto Exposure Control."""
        ...

    @exposure_ctrl.setter
    def exposure_ctrl(self, value: bool) -> None:
        ...

    @property
    def hmirror(self) -> bool:
        """Get/set horizontal mirror."""
        ...

    @hmirror.setter
    def hmirror(self, value: bool) -> None:
        ...

    @property
    def vflip(self) -> bool:
        """Get/set vertical flip."""
        ...

    @vflip.setter
    def vflip(self, value: bool) -> None:
        ...

    @property
    def aec2(self) -> bool:
        """Get/set AEC DSP."""
        ...

    @aec2.setter
    def aec2(self, value: bool) -> None:
        ...

    @property
    def awb_gain(self) -> bool:
        """Get/set Auto White Balance Gain."""
        ...

    @awb_gain.setter
    def awb_gain(self, value: bool) -> None:
        ...

    @property
    def agc_gain(self) -> int:
        """Get/set Auto Gain Control gain value."""
        ...

    @agc_gain.setter
    def agc_gain(self, value: int) -> None:
        ...

    @property
    def aec_value(self) -> int:
        """Get/set Auto Exposure Control value."""
        ...

    @aec_value.setter
    def aec_value(self, value: int) -> None:
        ...

    @property
    def special_effect(self) -> int:
        """Get/set special effect."""
        ...

    @special_effect.setter
    def special_effect(self, value: int) -> None:
        ...

    @property
    def wb_mode(self) -> int:
        """Get/set white balance mode."""
        ...

    @wb_mode.setter
    def wb_mode(self, value: int) -> None:
        ...

    @property
    def ae_level(self) -> int:
        """Get/set Auto Exposure level (-2 to 2)."""
        ...

    @ae_level.setter
    def ae_level(self, value: int) -> None:
        ...

    @property
    def dcw(self) -> bool:
        """Get/set DCW (Downsize EN)."""
        ...

    @dcw.setter
    def dcw(self, value: bool) -> None:
        ...

    @property
    def bpc(self) -> bool:
        """Get/set Bad Pixel Correction."""
        ...

    @bpc.setter
    def bpc(self, value: bool) -> None:
        ...

    @property
    def wpc(self) -> bool:
        """Get/set White Pixel Correction."""
        ...

    @wpc.setter
    def wpc(self, value: bool) -> None:
        ...

    @property
    def raw_gma(self) -> bool:
        """Get/set GMA (Gamma) Correction."""
        ...

    @raw_gma.setter
    def raw_gma(self, value: bool) -> None:
        ...

    @property
    def lenc(self) -> bool:
        """Get/set Lens Correction."""
        ...

    @lenc.setter
    def lenc(self, value: bool) -> None:
        ...

    # Core methods
    def reconfigure(self, *, frame_size: int | None = None,
                   pixel_format: int | None = None,
                   grab_mode: int | None = None,
                   fb_count: int | None = None) -> None:
        """Reconfigure camera with new settings."""
        ...

    def init(self) -> None:
        """Initialize the camera."""
        ...

    def deinit(self) -> None:
        """Deinitialize the camera."""
        ...

    def frame_available(self) -> bool:
        """Check if a frame is available."""
        ...

    def capture(self) -> memoryview:
        """Capture a frame and return it as a memoryview."""
        ...

    def free_buffer(self) -> None:
        """Free the frame buffer."""
        ...

    # Deprecated methods (use properties instead)
    def get_special_effect(self) -> int:
        """Deprecated: Use the special_effect property instead."""
        ...

    def set_special_effect(self, value: int) -> None:
        """Deprecated: Use the special_effect property instead."""
        ...

    def set_awb_gain(self, value: bool) -> None:
        """Deprecated: Use the awb_gain property instead."""
        ...

    def get_awb_gain(self) -> bool:
        """Deprecated: Use the awb_gain property instead."""
        ...

    def set_agc_gain(self, value: int) -> None:
        """Deprecated: Use the agc_gain property instead."""
        ...

    def get_agc_gain(self) -> int:
        """Deprecated: Use the agc_gain property instead."""
        ...

    def set_aec_value(self, value: int) -> None:
        """Deprecated: Use the aec_value property instead."""
        ...

    def get_aec_value(self) -> int:
        """Deprecated: Use the aec_value property instead."""
        ...

    def set_bpc(self, value: bool) -> None:
        """Deprecated: Use the bpc property instead."""
        ...

    def get_bpc(self) -> bool:
        """Deprecated: Use the bpc property instead."""
        ...

    def set_contrast(self, value: int) -> None:
        """Deprecated: Use the contrast property instead."""
        ...

    def get_contrast(self) -> int:
        """Deprecated: Use the contrast property instead."""
        ...

    def set_colorbar(self, value: bool) -> None:
        """Deprecated: Use the colorbar property instead."""
        ...

    def get_colorbar(self) -> bool:
        """Deprecated: Use the colorbar property instead."""
        ...

    def set_brightness(self, value: int) -> None:
        """Deprecated: Use the brightness property instead."""
        ...

    def get_brightness(self) -> int:
        """Deprecated: Use the brightness property instead."""
        ...

    def set_aec2(self, value: bool) -> None:
        """Deprecated: Use the aec2 property instead."""
        ...

    def get_aec2(self) -> bool:
        """Deprecated: Use the aec2 property instead."""
        ...

    def set_whitebal(self, value: bool) -> None:
        """Deprecated: Use the whitebal property instead."""
        ...

    def get_whitebal(self) -> bool:
        """Deprecated: Use the whitebal property instead."""
        ...

    def set_wb_mode(self, value: int) -> None:
        """Deprecated: Use the wb_mode property instead."""
        ...

    def get_wb_mode(self) -> int:
        """Deprecated: Use the wb_mode property instead."""
        ...

    def set_vflip(self, value: bool) -> None:
        """Deprecated: Use the vflip property instead."""
        ...

    def get_vflip(self) -> bool:
        """Deprecated: Use the vflip property instead."""
        ...

    def set_wpc(self, value: bool) -> None:
        """Deprecated: Use the wpc property instead."""
        ...

    def get_wpc(self) -> bool:
        """Deprecated: Use the wpc property instead."""
        ...

    def set_ae_level(self, value: int) -> None:
        """Deprecated: Use the ae_level property instead."""
        ...

    def get_ae_level(self) -> int:
        """Deprecated: Use the ae_level property instead."""
        ...

    def set_dcw(self, value: bool) -> None:
        """Deprecated: Use the dcw property instead."""
        ...

    def get_dcw(self) -> bool:
        """Deprecated: Use the dcw property instead."""
        ...

    def set_sharpness(self, value: int) -> None:
        """Deprecated: Use the sharpness property instead."""
        ...

    def get_sharpness(self) -> int:
        """Deprecated: Use the sharpness property instead."""
        ...

    def set_saturation(self, value: int) -> None:
        """Deprecated: Use the saturation property instead."""
        ...

    def get_saturation(self) -> int:
        """Deprecated: Use the saturation property instead."""
        ...

    def set_raw_gma(self, value: bool) -> None:
        """Deprecated: Use the raw_gma property instead."""
        ...

    def get_raw_gma(self) -> bool:
        """Deprecated: Use the raw_gma property instead."""
        ...

    def set_quality(self, value: int) -> None:
        """Deprecated: Use the quality property instead."""
        ...

    def get_quality(self) -> int:
        """Deprecated: Use the quality property instead."""
        ...

    def set_frame_size(self, value: int) -> None:
        """Deprecated: Use the frame_size property instead."""
        ...

    def get_frame_size(self) -> int:
        """Deprecated: Use the frame_size property instead."""
        ...

    def set_exposure_ctrl(self, value: bool) -> None:
        """Deprecated: Use the exposure_ctrl property instead."""
        ...

    def get_exposure_ctrl(self) -> bool:
        """Deprecated: Use the exposure_ctrl property instead."""
        ...

    def set_denoise(self, value: int) -> None:
        """Deprecated: Use the denoise property instead."""
        ...

    def get_denoise(self) -> int:
        """Deprecated: Use the denoise property instead."""
        ...

    def set_gain_ctrl(self, value: bool) -> None:
        """Deprecated: Use the gain_ctrl property instead."""
        ...

    def get_gain_ctrl(self) -> bool:
        """Deprecated: Use the gain_ctrl property instead."""
        ...

    def set_lenc(self, value: bool) -> None:
        """Deprecated: Use the lenc property instead."""
        ...

    def get_lenc(self) -> bool:
        """Deprecated: Use the lenc property instead."""
        ...

    def set_hmirror(self, value: bool) -> None:
        """Deprecated: Use the hmirror property instead."""
        ...

    def get_hmirror(self) -> bool:
        """Deprecated: Use the hmirror property instead."""
        ...

    def set_gainceiling(self, value: int) -> None:
        """Deprecated: Use the gainceiling property instead."""
        ...

    def get_gainceiling(self) -> int:
        """Deprecated: Use the gainceiling property instead."""
        ...

    def get_pixel_width(self) -> int:
        """Deprecated: Use the pixel_width property instead."""
        ...

    def get_pixel_height(self) -> int:
        """Deprecated: Use the pixel_height property instead."""
        ...

    def get_pixel_format(self) -> int:
        """Deprecated: Use the pixel_format property instead."""
        ...

    def get_sensor_name(self) -> str:
        """Deprecated: Use the sensor_name property instead."""
        ...

    def get_max_frame_size(self) -> int:
        """Deprecated: Use the max_frame_size property instead."""
        ...

    def get_fb_count(self) -> int:
        """Deprecated: Use the fb_count property instead."""
        ...

    def get_grab_mode(self) -> int:
        """Deprecated: Use the grab_mode property instead."""
        ...

