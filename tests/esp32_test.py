from camera import Camera, FrameSize, PixelFormat

def test_property_get_frame_size():
    cam = Camera()
    Frame_Size = FrameSize.VGA
    cam.reconfigure(frame_size=Frame_Size.VGA)
    assert cam.get_frame_size == Frame_Size
    assert cam.get_pixel_width == 640
    assert cam.get_pixel_height == 480

def test_property_get_pixel_format():
    cam = Camera()
    Pixel_Format = PixelFormat.RGB565
    cam.reconfigure(pixel_format=PixelFormat.RGB)
    assert cam.get_pixel_format == Pixel_Format

def test_camera_properties():
    cam = Camera()
    for name in dir(cam):
        if name.startswith('get_'):
            prop_name = name[4:]
            set_method_name = f'set_{prop_name}'
            if hasattr(cam, set_method_name):
                set_method = getattr(cam, set_method_name)
                get_method = getattr(cam, name)
                set_method(1)
                assert get_method() == 1, f"Failed for property {prop_name}"