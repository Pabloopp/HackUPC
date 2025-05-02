import cv2
import serial
import time

grey_channel = False
resize = True
SERIAL_PORT = '/dev/ttys001'

ser = serial.Serial(SERIAL_PORT, 115200)
time.sleep(5)

cap = cv2.VideoCapture(1)

while True:
    ret, frame = cap.read()

    if not ret:
        print("Can't receive frame (stream end?). Exiting ...")
        break

    if resize:
        frame = cv2.resize(frame, (64, 48))

    if grey_channel:
        # Convert to grayscale
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    cv2.imshow('Camera Feed', frame)

    # Encode the frame as JPEG
    _, encoded_frame = cv2.imencode('.jpg', frame)
    ser.write(encoded_frame.tobytes())

    # Exit on 'q' key press
    if cv2.waitKey(1) == ord('q'):
        break

# When everything done, release the capture
ser.close()
cap.release()
cv2.destroyAllWindows()