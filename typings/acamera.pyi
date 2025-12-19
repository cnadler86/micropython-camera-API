from camera import Camera as _Camera
from camera import FrameSize, PixelFormat, GainCeiling, GrabMode

class Camera(_Camera):
    async def acapture(self) -> memoryview:
        """Asynchronously capture a frame and return it as a memoryview.
        
        Yields control back to the event loop while waiting for a frame to become available.
        """
        ...

__all__ = ['Camera', 'FrameSize', 'PixelFormat', 'GainCeiling', 'GrabMode']