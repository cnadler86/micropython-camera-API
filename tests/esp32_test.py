import time
from camera import Camera, FrameSize, PixelFormat

def test_property_get_frame_size():
    with Camera() as cam:
        print("Test frame size")
        Frame_Size = FrameSize.VGA
        cam.reconfigure(frame_size=Frame_Size)
        assert cam.get_frame_size() == Frame_Size
        assert cam.get_pixel_width() == 640
        assert cam.get_pixel_height() == 480

def test_property_get_pixel_format():
    with Camera() as cam:
        print("Test pixel format")
        for Pixel_Format_Name in dir(PixelFormat):
            Pixel_Format = getattr(PixelFormat, Pixel_Format_Name)
            try:
                if Pixel_Format_Name.startswith("_") or Pixel_Format_Name.startswith("RGB888"):
                    continue
                cam.reconfigure(pixel_format=Pixel_Format)
                assert cam.get_pixel_format() == Pixel_Format
            except Exception:
                print("\tFailed test for pixel format", Pixel_Format)

def test_must_be_initialized():
    with Camera(init=False) as cam:
        print(f"Testing capture without initalization")
        try:
            cam.capture()
            assert False, "Capture should have failed"
        except Exception as e:
            if e == "Camera not initialized":
                assert False, "Capture should have failed"
            else:
                assert True
                
def test_camera_properties():
    with Camera() as cam:
        print(f"Testing get/set methods")
        for name in dir(cam):
            if name.startswith('get_'):
                prop_name = name[4:]
                set_method_name = f'set_{prop_name}'
                if hasattr(cam, set_method_name):
                    set_method = getattr(cam, set_method_name)
                    get_method = getattr(cam, name)
                    try:
                        set_method(1)
                        assert get_method() == 1
                    except Exception:
                        print("\tFailed test for property", prop_name)

def test_invalid_settings():
    print(f"Testing invalid settings")
    invalid_settings = [
        {"xclk_freq": 21000000},
        {"frame_size": 23},
        {"pixel_format": 7},
        {"jpeg_quality": 101},
        {"jpeg_quality": -1},
        {"grab_mode": 3},
    ]
    Delay= 10

    for settings in invalid_settings:
        param, invalid_value = next(iter(settings.items()))
        try:
            print("testing",param,"=",invalid_value)
            time.sleep_ms(Delay)
            with Camera(**{param: invalid_value}) as cam:
                print(f"\tFailed test for {param} with value {invalid_value}")
        except Exception as e:
            time.sleep_ms(Delay)

if __name__ == "__main__":
    test_property_get_frame_size()
    test_property_get_pixel_format()
    test_must_be_initialized()
    test_camera_properties()
    test_invalid_settings()

