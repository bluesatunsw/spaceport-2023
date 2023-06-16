import serial
import argparse
import os
import time

# parse commandline argument for serial port
parser = argparse.ArgumentParser()
parser.add_argument("port", help="The device port to listen to. Example: /dev/ttyS0")
parser.add_argument("start_message", help="The initial message to wait for before starting file transmission.")
args = parser.parse_args()

# open serial port
ser = serial.Serial(args.port, baudrate=112500, timeout=1)

current_file = None
while True:
    line = ser.readline().decode('utf-8').strip()

    if args.start_message in line:
        print("Received start message. Starting to read files.")
        ser.write(b'1')  # Send acknowledgement character 'A'

    elif line.startswith('\\FILE START'):
        # Extract file name from the line
        file_name = line.split()[2]
        print(f"Starting to read file: {file_name}")
        current_file = open(file_name, 'wb')

    elif line == '\\FILE END\\':
        print("Finished reading file.")
        if current_file is not None:
            current_file.close()
            current_file = None

    elif current_file is not None:
        # Write bytes to the current file
        current_file.write(line.encode('utf-8'))

    else:
        # If no file is open and the line doesn't match any special command, print it with "HOST: " prepended
        print(f"HOST: {line}")

    time.sleep(0.1)  # small delay to prevent CPU overload
