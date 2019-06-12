import serial
import time
channel = 3
degree = 3081
with serial.Serial('/dev/cu.usbserial-DN05J65H', 115200) as ser:
    writer = u'{0}\0'.format(degree)
    ser.write(writer.encode('utf-8'))

with serial.Serial('/dev/cu.usbserial-DN05J65H', 115200) as ser:
    read = ser.readlines()
    read = read.strip().decode('utf-8')
    print(read)
