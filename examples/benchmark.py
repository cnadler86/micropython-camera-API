from camera import Camera, FrameSize, PixelFormat
import time
import gc
import os
gc.enable()

def measure_fps(duration=2):
    start_time = time.time()
    while time.time() - start_time < 0.5:
        cam.capture()
    
    start_time = time.time()
    frame_count = 0

    while time.time() - start_time < duration:
        cam.capture()
        frame_count += 1

    end_time = time.time()
    fps = frame_count / (end_time - start_time)
    return fps

def print_summary_table(results, cam):
    (_,_,_,_,target) = os.uname()
    print(f"\nBenchmark {target} with {cam.get_sensor_name()}, fb_count: {cam.get_fb_count()}, GrabMode: {cam.get_grab_mode()}:")
    
    pixel_formats = list(results.keys())
    print(f"{'Frame Size':<15}", end="")
    for p in pixel_formats:
        print(f"{p:<15}", end="")
    print()

    frame_size_names = {getattr(FrameSize, f): f for f in dir(FrameSize) if not f.startswith('_')}
    frame_sizes = list(next(iter(results.values())).keys())
    
    for f in frame_sizes:
        frame_size_name = frame_size_names.get(f, str(f))
        print(f"{frame_size_name:<15}", end="")
        for p in pixel_formats:
            fps = results[p].get(f, "N/A")
            print(f"{fps:<15}", end="")
        print()

if __name__ == "__main__":
    cam = Camera()
    results = {}

    try:
        for p in dir(PixelFormat):
            if not p.startswith('_'):
                p_value = getattr(PixelFormat, p)
                if p_value == PixelFormat.RGB888 and cam.get_sensor_name() == "OV2640":
                    continue
                try:
                    cam.reconfigure(pixel_format=p_value)
                    results[p] = {}
                except Exception as e:
                    print('ERR:', e)
                    continue

                for f in dir(FrameSize):
                    if not f.startswith('_'):
                        f_value = getattr(FrameSize, f)
                        if f_value > cam.get_max_frame_size():
                            continue
                        gc.collect()
                        print('Set', p, f, ':')

                        try:
                            cam.reconfigure(frame_size=f_value)
                            time.sleep_ms(10)
                            img = cam.capture()
                            
                            if img:
                                print('---> Image size:', len(img))
                                fps = measure_fps(2)
                                print(f"---> FPS: {fps}")
                                results[p][f_value] = fps  # FPS in Dictionary speichern
                            else:
                                print('No image captured')
                                results[p][f_value] = 'No image'
                            
                            print(f"---> Free Memory: {gc.mem_free()}")
                        except Exception as e:
                            print('ERR:', e)
                            results[p][f_value] = 'ERR'
                        finally:
                            time.sleep_ms(250)
                            gc.collect()
                            print('')

    except KeyboardInterrupt:
        print("\nScript interrupted by user.")

    finally:
        cam.deinit()
        print_summary_table(results)  # Tabelle am Ende ausgeben

