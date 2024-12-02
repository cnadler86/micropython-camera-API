from camera import Camera, FrameSize, PixelFormat
import time
import gc
import os
gc.enable()

def measure_fps(cam,out_fmt,duration=2):
    start_time = time.ticks_ms()
    while time.ticks_ms() - start_time < 500:
        cam.capture(out_fmt)
    
    start_time = time.ticks_ms()
    frame_count = 0

    while time.ticks_ms() - start_time < duration*1000:
        img = cam.capture(out_fmt)
        if img:
            frame_count += 1

    end_time = time.ticks_ms()
    fps = frame_count / (end_time - start_time) * 1000
    return round(fps,1)

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
    cam = Camera(pixel_format=PixelFormat.JPEG)
    results = {}

    try:
        for fb in [1, 2]:
            cam.reconfigure(fb_count=fb, frame_size=FrameSize.QQVGA)
            results[fb] = {}
            for p in dir(PixelFormat):
                if not p.startswith('_'):
                    p_value = getattr(PixelFormat, p)
                    try:
                        if p_value == PixelFormat.JPEG:
                            continue
                        cam.capture(p_value)
                        results[fb][p] = {}
                        gc.collect()
                    except:
                        continue
                    for f in dir(FrameSize):
                        if not f.startswith('_'):
                            f_value = getattr(FrameSize, f)
                            if f_value > cam.get_max_frame_size():
                                continue
                            gc.collect()
                            print('Set', p, f,f'fb={fb}',':')

                            try:
                                cam.set_frame_size(f_value)
                                time.sleep_ms(10)
                                img = cam.capture(p_value)
                                if img:
                                    print('---> Image size:', len(img))
                                    fps = measure_fps(cam,p_value,2)
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
