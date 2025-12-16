import network
import asyncio
import time
from acamera import Camera, FrameSize, PixelFormat # Import the async version of the Camera class, you can also use the sync version (camera.Camera)

cam = Camera(frame_size=FrameSize.VGA, pixel_format=PixelFormat.JPEG, jpeg_quality=85, init=False)
# WLAN config
ssid = '<yourSSID>'
password = '<yourPW>'

station = network.WLAN(network.STA_IF)
station.active(True)
station.connect(ssid, password)

while not station.isconnected():
    time.sleep(1)

print(f'Connected! IP: {station.ifconfig()[0]}. Open this IP in your browser')

try:
    with open("CameraSettings.html", 'r') as file:
        html = file.read()
except Exception as e:
    print("Error reading CameraSettings.html file. You might forgot to copy it from the examples folder.")
    raise e
async def stream_camera(writer):
    try:
        cam.init()
        await asyncio.sleep(1)
        
        writer.write(b'HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n')
        await writer.drain()

        while True:
            frame = await cam.acapture() # This is the async version of capture, you can also use frame = cam.capture() instead
            if frame:
                writer.write(b'--frame\r\nContent-Type: image/jpeg\r\n\r\n')
                writer.write(frame)
                await writer.drain()
                
    finally:
        cam.deinit()
        writer.close()
        await writer.wait_closed()
        print("Streaming stopped and camera deinitialized.")

async def handle_client(reader, writer):
    try:
        request = await reader.read(1024)
        request = request.decode()

        if 'GET /stream' in request:
            print("Start streaming...")
            await stream_camera(writer)

        elif 'GET /set_' in request:
            method_name = request.split('GET /set_')[1].split('?')[0]
            value = int(request.split('value=')[1].split(' ')[0])
            try:
                # Use property assignment instead of method call
                setattr(cam, method_name, value)
                print(f"setting {method_name} to {value}")
                response = 'HTTP/1.1 200 OK\r\n\r\n'
                writer.write(response.encode())
                await writer.drain()
            except (AttributeError, ValueError) as e:
                # Fallback to reconfigure if property doesn't exist
                try:
                    cam.reconfigure(**{method_name: value})
                    print(f"Camera reconfigured with {method_name}={value}")
                    print("This action restores all previous configuration!")
                    response = 'HTTP/1.1 200 OK\r\n\r\n'
                    writer.write(response.encode())
                    await writer.drain()
                except Exception as e2:
                    print(f"Error setting {method_name}: {e2}")
                    response = 'HTTP/1.1 404 Not Found\r\n\r\n'
                    writer.write(response.encode())
                    await writer.drain()

        elif 'GET /get_' in request:
            method_name = request.split('GET /get_')[1].split(' ')[0]
            try:
                # Use property access instead of method call
                value = getattr(cam, method_name)
                print(f"{method_name} is {value}")
                response = f'HTTP/1.1 200 OK\r\n\r\n{value}'
                writer.write(response.encode())
                await writer.drain()
            except AttributeError as e:
                print(f"Error getting {method_name}: {e}")
                response = 'HTTP/1.1 404 Not Found\r\n\r\n'
                writer.write(response.encode())
                await writer.drain()

        else:
            writer.write('HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n'.encode() + html.encode())
            await writer.drain()
    except Exception as e:
        print(f"Error: {e}")
    finally:
        writer.close()
        await writer.wait_closed()

async def start_server():
    server = await asyncio.start_server(handle_client, "0.0.0.0", 80)
    print(f'Server is running on {station.ifconfig()[0]}:80')
    while True:
        await asyncio.sleep(3600)

try:
    asyncio.run(start_server())
except KeyboardInterrupt:
    cam.deinit()
    print("Server stopped")

