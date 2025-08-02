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
                 pixel_format: int = PixelFormat.RGB565,
                 frame_size: int = FrameSize.QVGA,
                 jpeg_quality: int = 85,
                 fb_count: int = 1,
                 grab_mode: int = GrabMode.WHEN_EMPTY,
                 init: bool = True) -> None:
        ...

    def get_special_effect(self) -> int:
        """Get the current special effect setting."""
        ...

    def set_special_effect(self, value: int) -> None:
        """Set the special effect."""
        ...

    def set_awb_gain(self, value: bool) -> None:
        """Enable/disable Auto White Balance Gain."""
        ...

    def get_awb_gain(self) -> bool:
        """Get Auto White Balance Gain state."""
        ...

    def set_agc_gain(self, value: int) -> None:
        """Set the Auto Gain Control gain value."""
        ...

    def get_agc_gain(self) -> int:
        """Get the Auto Gain Control gain value."""
        ...

    def set_aec_value(self, value: int) -> None:
        """Set the Auto Exposure Control value."""
        ...

    def get_aec_value(self) -> int:
        """Get the Auto Exposure Control value."""
        ...

    def set_bpc(self, value: bool) -> None:
        """Enable/disable Bad Pixel Correction."""
        ...

    def get_bpc(self) -> bool:
        """Get Bad Pixel Correction state."""
        ...

    def set_contrast(self, value: int) -> None:
        """Set the contrast level (-2 to 2)."""
        ...

    def get_contrast(self) -> int:
        """Get the contrast level."""
        ...

    def set_colorbar(self, value: bool) -> None:
        """Enable/disable color bar test pattern."""
        ...

    def get_colorbar(self) -> bool:
        """Get color bar test pattern state."""
        ...

    def set_brightness(self, value: int) -> None:
        """Set the brightness level (-2 to 2)."""
        ...

    def get_brightness(self) -> int:
        """Get the brightness level."""
        ...

    def set_aec2(self, value: bool) -> None:
        """Enable/disable AEC DSP."""
        ...

    def get_aec2(self) -> bool:
        """Get AEC DSP state."""
        ...

    def set_whitebal(self, value: bool) -> None:
        """Enable/disable white balance."""
        ...

    def get_whitebal(self) -> bool:
        """Get white balance state."""
        ...

    def set_wb_mode(self, value: int) -> None:
        """Set white balance mode."""
        ...

    def get_wb_mode(self) -> int:
        """Get white balance mode."""
        ...

    def set_vflip(self, value: bool) -> None:
        """Enable/disable vertical flip."""
        ...

    def get_vflip(self) -> bool:
        """Get vertical flip state."""
        ...

    def set_wpc(self, value: bool) -> None:
        """Enable/disable White Pixel Correction."""
        ...

    def get_wpc(self) -> bool:
        """Get White Pixel Correction state."""
        ...

    def set_ae_level(self, value: int) -> None:
        """Set Auto Exposure level (-2 to 2)."""
        ...

    def get_ae_level(self) -> int:
        """Get Auto Exposure level."""
        ...

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

    def set_dcw(self, value: bool) -> None:
        """Enable/disable DCW (Downsize EN)."""
        ...

    def get_dcw(self) -> bool:
        """Get DCW (Downsize EN) state."""
        ...

    def set_sharpness(self, value: int) -> None:
        """Set the sharpness level (-2 to 2)."""
        ...

    def get_sharpness(self) -> int:
        """Get the sharpness level."""
        ...

    def set_saturation(self, value: int) -> None:
        """Set the saturation level (-2 to 2)."""
        ...

    def get_saturation(self) -> int:
        """Get the saturation level."""
        ...

    def set_raw_gma(self, value: bool) -> None:
        """Enable/disable GMA (Gamma) Correction."""
        ...

    def get_raw_gma(self) -> bool:
        """Get GMA (Gamma) Correction state."""
        ...

    def set_quality(self, value: int) -> None:
        """Set JPEG quality (0-63)."""
        ...

    def get_quality(self) -> int:
        """Get JPEG quality."""
        ...

    def set_frame_size(self, value: int) -> None:
        """Set frame size."""
        ...

    def get_frame_size(self) -> int:
        """Get frame size."""
        ...

    def set_exposure_ctrl(self, value: bool) -> None:
        """Enable/disable Auto Exposure Control."""
        ...

    def get_exposure_ctrl(self) -> bool:
        """Get Auto Exposure Control state."""
        ...

    def set_denoise(self, value: int) -> None:
        """Set denoise level."""
        ...

    def get_denoise(self) -> int:
        """Get denoise level."""
        ...

    def set_gain_ctrl(self, value: bool) -> None:
        """Enable/disable Auto Gain Control."""
        ...

    def get_gain_ctrl(self) -> bool:
        """Get Auto Gain Control state."""
        ...

    def set_lenc(self, value: bool) -> None:
        """Enable/disable Lens Correction."""
        ...

    def get_lenc(self) -> bool:
        """Get Lens Correction state."""
        ...

    def set_hmirror(self, value: bool) -> None:
        """Enable/disable horizontal mirror."""
        ...

    def get_hmirror(self) -> bool:
        """Get horizontal mirror state."""
        ...

    def set_gainceiling(self, value: int) -> None:
        """Set gain ceiling."""
        ...

    def get_gainceiling(self) -> int:
        """Get gain ceiling."""
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

    def get_pixel_width(self) -> int:
        """Get frame width in pixels."""
        ...

    def get_pixel_height(self) -> int:
        """Get frame height in pixels."""
        ...

    def get_pixel_format(self) -> int:
        """Get current pixel format."""
        ...

    def get_sensor_name(self) -> str:
        """Get camera sensor name."""
        ...

    def get_max_frame_size(self) -> int:
        """Get maximum supported frame size."""
        ...

    def get_fb_count(self) -> int:
        """Get frame buffer count."""
        ...

    def get_grab_mode(self) -> int:
        """Get current grab mode."""
        ...

