import serial
import numpy as np
import cv2

SERIAL_PORT = '/dev/ttys002'
BAUD_RATE = 115200

def decode_serial_flux():
    try:
        # Open the serial connection
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1) as ser:
            print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud.")
            buffer = bytearray()

            while True:
                # Read data from the serial port
                if ser.in_waiting > 0:
                    buffer.extend(ser.read(ser.in_waiting))

                    while True:
                        # Look for the first valid JPEG start and end markers
                        start_idx = buffer.find(b'\xff\xd8')  # First start marker
                        end_idx = buffer.find(b'\xff\xd9')   # First end marker

                        # Ensure valid start and end markers exist and are in the correct order
                        if start_idx != -1 and end_idx != -1 and end_idx > start_idx:
                            # Extract the JPEG frame
                            jpeg_data = buffer[start_idx:end_idx + 2]
                            buffer = buffer[end_idx + 2:]  # Remove processed data

                            # Decode the JPEG frame
                            frame = cv2.imdecode(np.frombuffer(jpeg_data, dtype=np.uint8), cv2.IMREAD_COLOR)

                            if frame is not None:
                                # Display the frame
                                cv2.imshow('Decoded Frame', frame)

                                # Exit on 'q' key press
                                if cv2.waitKey(1) == ord('q'):
                                    return
                            else:
                                print("Failed to decode frame.")
                        else:
                            # No valid frame found, exit the inner loop
                            break
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("\nDecoder interrupted by user.")
    finally:
        cv2.destroyAllWindows()

if __name__ == "__main__":
    decode_serial_flux()