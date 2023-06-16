import serial
import time

# Serial port configuration
port = "/dev/ttyACM0"  # Replace with your port
baudrate = 9600

# Open the serial port
ser = serial.Serial(port, baudrate)

# Ensure the serial port is open
if ser.isOpen():
    print("Serial port is open.")
else:
    print("Could not open serial port.")

# Wait for a moment before sending
time.sleep(1)

# Send a single character '1' to the serial port
while True:
    ser.write(b'HELLO')
    line = ser.readline()
    print(line)

# Close the serial port
ser.close()

print("Data sent.")