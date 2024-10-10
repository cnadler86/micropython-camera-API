from camera import Camera, FrameSize, PixelFormat

def test_property_get_frame_size():
    camera = Camera()
    Frame_Size = FrameSize.VGA
    camera.reconfigure(frame_size=Frame_Size.VGA)
    assert camera.get_frame_size == Frame_Size
    assert camera.get_pixel_width == 640
    assert camera.get_pixel_height == 480

def test_property_get_pixel_format():
    camera = Camera()
    Pixel_Format = PixelFormat.RGB565
    camera.reconfigure(pixel_format=PixelFormat.RGB)
    assert camera.get_pixel_format == Pixel_Format

def test_camera_properties():
    camera = Camera()
    for name in dir(camera):
        if name.startswith('get_'):
            prop_name = name[4:]
            set_method_name = f'set_{prop_name}'
            if hasattr(camera, set_method_name):
                set_method = getattr(camera, set_method_name)
                get_method = getattr(camera, name)
                set_method(1)
                assert get_method() == 1, f"Failed for property {prop_name}"