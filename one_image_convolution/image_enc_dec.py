import serial

INPUT_IMAGE_PATH = 'input.jpg'
OUTPUT_IMAGE_PATH = 'output.jpg'
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Send image via serial
with open(INPUT_IMAGE_PATH, 'rb') as f:
    img_data = f.read()
    #ser.write(len(img_data).to_bytes(4, 'big')) # Send size
    ser.write(img_data)

# Receive image via serial
received_data = b''

while True:
    chunk = ser.read(1024)
    if not chunk:
        continue
    received_data += chunk

    # Check for JPEG end-of-image marker
    if b'\xFF\xD9' in received_data[-2:]:
        break

eoi_index = received_data.find(b'\xFF\xD9')
if eoi_index != -1:
    received_data = received_data[:eoi_index + 2]

# Save the received image
with open(OUTPUT_IMAGE_PATH, 'wb') as f:
    f.write(received_data)

ser.close()