import serial
import cv2
import numpy as np

INPUT_IMAGE_PATH = 'input2.jpg'
OUTPUT_IMAGE_PATH = 'output2.jpg'
SERIAL_PORT = '/dev/ttyUSB1'
BAUD_RATE = 115200
SERIAL_TIMEOUT = 5

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=SERIAL_TIMEOUT)
ser.reset_input_buffer()
ser.reset_output_buffer()
image = cv2.imread(INPUT_IMAGE_PATH)
image = cv2.resize(image, (0, 0), fx=0.3, fy=0.3)
print("Image read from file: ", INPUT_IMAGE_PATH)
if image is None:
    print("Error: Could not read the image.")
    exit()

# Serialize the image
height, width, channels = image.shape
print(height.to_bytes(4, 'big'))
header = height.to_bytes(4, 'big') + width.to_bytes(4, 'big')
serialized_image = header + image.flatten().tobytes()
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
expected_bytes = num_pixels * 3

data = bytearray()
while len(data) < expected_bytes:
    chunk = ser.read(expected_bytes - len(data))
    if not chunk:
        print("Error: Timeout or disconnection during image reception.")
        exit()
    data.extend(chunk)

received_image = np.frombuffer(data, dtype=np.uint8).reshape((recv_height, recv_width, 3))

cv2.imwrite(OUTPUT_IMAGE_PATH, received_image)
print("Received image saved to ", OUTPUT_IMAGE_PATH)
