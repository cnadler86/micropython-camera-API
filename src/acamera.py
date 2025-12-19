import asyncio
from camera import Camera as _Camera
from camera import FrameSize, PixelFormat, GainCeiling, GrabMode

class Camera(_Camera):
    """
    Async wrapper for Camera class.
    
    Workaround for MicroPython bug: Python subclasses of native types with attr handlers
    cannot properly set attributes. The attr handler is bypassed and values are stored in
    the instance __dict__ instead. We work around this by explicitly defining properties
    that delegate to the get_*/set_* methods which are in locals_dict and work correctly.
    
    See: https://github.com/micropython/micropython/issues/18592
    """
    
    async def acapture(self):
        self.free_buffer() # Free the buffer so the camera task grabs a new frame
        while not self.frame_available():
            await asyncio.sleep(0) # Yield control to the event loop
        return self.capture()
    
    # Workaround: Define properties that delegate to get_/set_ methods
    # These methods are in locals_dict and work correctly with subclasses
    
    @property
    def frame_size(self):
        return self.get_frame_size()
    @frame_size.setter
    def frame_size(self, value):
        self.set_frame_size(value)
    
    @property
    def contrast(self):
        return self.get_contrast()
    @contrast.setter
    def contrast(self, value):
        self.set_contrast(value)
    
    @property
    def brightness(self):
        return self.get_brightness()
    @brightness.setter
    def brightness(self, value):
        self.set_brightness(value)
    
    @property
    def saturation(self):
        return self.get_saturation()
    @saturation.setter
    def saturation(self, value):
        self.set_saturation(value)
    
    @property
    def sharpness(self):
        return self.get_sharpness()
    @sharpness.setter
    def sharpness(self, value):
        self.set_sharpness(value)
    
    @property
    def denoise(self):
        return self.get_denoise()
    @denoise.setter
    def denoise(self, value):
        self.set_denoise(value)
    
    @property
    def gainceiling(self):
        return self.get_gainceiling()
    @gainceiling.setter
    def gainceiling(self, value):
        self.set_gainceiling(value)
    
    @property
    def quality(self):
        return self.get_quality()
    @quality.setter
    def quality(self, value):
        self.set_quality(value)
    
    @property
    def colorbar(self):
        return self.get_colorbar()
    @colorbar.setter
    def colorbar(self, value):
        self.set_colorbar(value)
    
    @property
    def whitebal(self):
        return self.get_whitebal()
    @whitebal.setter
    def whitebal(self, value):
        self.set_whitebal(value)
    
    @property
    def gain_ctrl(self):
        return self.get_gain_ctrl()
    @gain_ctrl.setter
    def gain_ctrl(self, value):
        self.set_gain_ctrl(value)
    
    @property
    def exposure_ctrl(self):
        return self.get_exposure_ctrl()
    @exposure_ctrl.setter
    def exposure_ctrl(self, value):
        self.set_exposure_ctrl(value)
    
    @property
    def hmirror(self):
        return self.get_hmirror()
    @hmirror.setter
    def hmirror(self, value):
        self.set_hmirror(value)
    
    @property
    def vflip(self):
        return self.get_vflip()
    @vflip.setter
    def vflip(self, value):
        self.set_vflip(value)
    
    @property
    def aec2(self):
        return self.get_aec2()
    @aec2.setter
    def aec2(self, value):
        self.set_aec2(value)
    
    @property
    def awb_gain(self):
        return self.get_awb_gain()
    @awb_gain.setter
    def awb_gain(self, value):
        self.set_awb_gain(value)
    
    @property
    def agc_gain(self):
        return self.get_agc_gain()
    @agc_gain.setter
    def agc_gain(self, value):
        self.set_agc_gain(value)
    
    @property
    def aec_value(self):
        return self.get_aec_value()
    @aec_value.setter
    def aec_value(self, value):
        self.set_aec_value(value)
    
    @property
    def special_effect(self):
        return self.get_special_effect()
    @special_effect.setter
    def special_effect(self, value):
        self.set_special_effect(value)
    
    @property
    def wb_mode(self):
        return self.get_wb_mode()
    @wb_mode.setter
    def wb_mode(self, value):
        self.set_wb_mode(value)
    
    @property
    def ae_level(self):
        return self.get_ae_level()
    @ae_level.setter
    def ae_level(self, value):
        self.set_ae_level(value)
    
    @property
    def dcw(self):
        return self.get_dcw()
    @dcw.setter
    def dcw(self, value):
        self.set_dcw(value)
    
    @property
    def bpc(self):
        return self.get_bpc()
    @bpc.setter
    def bpc(self, value):
        self.set_bpc(value)
    
    @property
    def wpc(self):
        return self.get_wpc()
    @wpc.setter
    def wpc(self, value):
        self.set_wpc(value)
    
    @property
    def raw_gma(self):
        return self.get_raw_gma()
    @raw_gma.setter
    def raw_gma(self, value):
        self.set_raw_gma(value)
    
    @property
    def lenc(self):
        return self.get_lenc()
    @lenc.setter
    def lenc(self, value):
        self.set_lenc(value)
    
__all__ = ['Camera', 'FrameSize', 'PixelFormat', 'GainCeiling', 'GrabMode']