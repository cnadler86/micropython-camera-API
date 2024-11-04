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
    print(f"\nBenchmark {os.uname().machine} with {cam.get_sensor_name()}, GrabMode: {cam.get_grab_mode()}:")

    fb_counts = sorted(results.keys())
    frame_size_names = {getattr(FrameSize, f): f for f in dir(FrameSize) if not f.startswith('_')}
    
    header_row = f"{'Frame Size':<15}"
    sub_header_row = " " * 15
    
    for fb in fb_counts:
        for p in results[fb].keys():
            header_row += f"{'fb_count ' + str(fb):<15}"
            sub_header_row += f"{p:<15}"
    
    print(header_row)
    print(sub_header_row)

    frame_sizes = list(next(iter(next(iter(results.values())).values())).keys())
    
    for f in frame_sizes:
        frame_size_name = frame_size_names.get(f, str(f))
        print(f"{frame_size_name:<15}", end="")

        for fb in fb_counts:
            for p in results[fb].keys():
                fps = results[fb][p].get(f, "N/A")
                print(f"{fps:<15}", end="")
        print()

if __name__ == "__main__":
    cam = Camera()
    results = {}

    try:
        for fb in [1, 2]:
            cam.reconfigure(fb_count=fb)
            results[fb] = {}
            for p in dir(PixelFormat):
                if not p.startswith('_'):
                    p_value = getattr(PixelFormat, p)
                    if (p_value == PixelFormat.RGB888 and cam.get_sensor_name() == "OV2640") or (p_value != PixelFormat.JPEG and fb > 1):
                        continue
                    try:
                        cam.reconfigure(pixel_format=p_value)
                        results[fb][p] = {}
                    except Exception as e:
                        print('ERR:', e)
                        continue

                    for f in dir(FrameSize):
                        if not f.startswith('_'):
                            f_value = getattr(FrameSize, f)
                            if f_value > cam.get_max_frame_size():
                                continue
                            gc.collect()
                            print('Set', p, f,f'fb={fb}',':')

                            try:
                                cam.reconfigure(frame_size=f_value)
                                time.sleep_ms(10)
                                img = cam.capture()
                                
                                if img:
                                    print('---> Image size:', len(img))
                                    fps = measure_fps(2)
                                    print(f"---> FPS: {fps}")
                                    results[fb][p][f_value] = fps
                                else:
                                    print('No image captured')
                                    results[fb][p][f_value] = 'No img'
                                
                                print(f"---> Free Memory: {gc.mem_free()}")
                            except Exception as e:
                                print('ERR:', e)
                                results[fb][p][f_value] = 'ERR'
                            finally:
                                time.sleep_ms(250)
                                gc.collect()
                                print('')

    except KeyboardInterrupt:
        print("\nScript interrupted by user.")

    finally:
        print_summary_table(results, cam)
        cam.deinit()
