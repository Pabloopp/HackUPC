import serial
import cv2

SERIAL_PORT = '/dev/tty.usbserial-1234_tul1'
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

                    # Iterate over the buffer and print each byte
                    for byte in buffer:
                        print(f"Byte: {byte}")

                    # Clear the buffer after printing
                    buffer.clear()

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("\nDecoder interrupted by user.")
    finally:
        cv2.destroyAllWindows()

if __name__ == "__main__":
    decode_serial_flux()