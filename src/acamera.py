import asyncio
from camera import Camera as _Camera
from camera import FrameSize, PixelFormat, GainCeiling, GrabMode

class Camera(_Camera):
    async def acapture(self):
        self.free_buffer() # Free the buffer so the camera task grabs a new frame
        while not self.frame_available():
            await asyncio.sleep(0) # Yield control to the event loop
        return self.capture()
    
__all__ = ['Camera', 'FrameSize', 'PixelFormat', 'GainCeiling', 'GrabMode']