import network
import socket
import time

from camera import Camera, FrameSize, PixelFormat
# Cam Config
cam = Camera(frame_size = FrameSize.VGA,pixel_format=PixelFormat.JPEG,init=False)

# WLAN config
ssid = '<yourSSID>'
password = '<yourPW>'

station = network.WLAN(network.STA_IF)
station.active(True)
station.connect(ssid, password)

while not station.isconnected():
    time.sleep(1)

print(f'Connected! IP: {station.ifconfig()[0]}. Open this IP in your browser')

# HTTP-Server starten
addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(addr)
s.listen(1)

print('Server on:', addr)

html = """<!DOCTYPE html>
<html>
<head>
    <title>Micropython Camera Stream</title>
    <style>
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        .video-container {
            text-align: center;
            width: 100%;
            max-width: 1000px; /* Max width of the container */
        }
        img {
            width: 100%;
            height: auto;
        }
    </style>
</head>
<body>
    <div class="video-container">
        <h1>ESP32 Camera Stream</h1>
        <img src="/stream">
    </div>
</body>
</html>
"""

def handle_client(client):
    try:
        request = client.recv(1024).decode()
        if 'GET /stream' in request:
            response = b'HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n'
            client.send(response)
            cam.init()
            while cam:
                frame = cam.capture()
                if frame:
                    client.send(b'--frame\r\n')
                    client.send(b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
                else:
                    break
        else:
            response = 'HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n' + html
            client.send(response.encode())
    except OSError as e:
        print('Error:', e)
    finally:
        client.close()
        cam.deinit()

while True:
    client, addr = s.accept()
    print('Connection from:', addr)
    handle_client(client)
