import serial
import cv2
import numpy as np

INPUT_IMAGE_PATH = 'input.jpg'
OUTPUT_IMAGE_PATH = 'output.jpg'
SERIAL_PORT = '/dev/ttyUSB1'
BAUD_RATE = 115200
SERIAL_TIMEOUT = 5

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=SERIAL_TIMEOUT)
ser.reset_input_buffer()
ser.reset_output_buffer()
image = cv2.imread(INPUT_IMAGE_PATH)
image = cv2.resize(image, (0, 0), fx=0.1, fy=0.1)
print("Image read from file: ", INPUT_IMAGE_PATH)

if image is None:
    print("Error: Could not read the image.")
    exit()

# Serialize the image
height, width, channels = image.shape
b_channel, g_channel, r_channel = cv2.split(image)
print(height.to_bytes(4, 'big'))
serialized_image = (height.to_bytes(4, 'big') + width.to_bytes(4, 'big')
                    + r_channel.tobytes() + g_channel.tobytes() + b_channel.tobytes())
print("Image size: ", height, width, channels)
ser.write(serialized_image)
print("Image sent to serial port.")

# Read the serialized image from the serial port
header = ser.read(8)
print(header)
if len(header) < 8:
    print("Error: Incomplete header received.")
    exit()

recv_height = int.from_bytes(header[:4], 'big')
recv_width = int.from_bytes(header[4:], 'big')
num_pixels = recv_height * recv_width
print(recv_height)
expected_bytes = num_pixels * 3

data = bytearray()
while len(data) < expected_bytes:
    chunk = ser.read(expected_bytes - len(data))
    if not chunk:
        print("Error: Timeout or disconnection during image reception.")
        exit()
    data.extend(chunk)

r = np.frombuffer(data[:num_pixels], dtype=np.uint8).reshape((recv_height, recv_width))
g = np.frombuffer(data[num_pixels:2*num_pixels], dtype=np.uint8).reshape((recv_height, recv_width))
b = np.frombuffer(data[2*num_pixels:], dtype=np.uint8).reshape((recv_height, recv_width))

received_image = cv2.merge([b, g, r])

cv2.imwrite(OUTPUT_IMAGE_PATH, received_image)
print("Received image saved to ", OUTPUT_IMAGE_PATH)
