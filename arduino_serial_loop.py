import serial
import time

ser = serial.Serial(port='/dev/ttyUSB0', baudrate=9600)

while True:
    time.sleep(1)
    print('input')
    writer = u'{0}'.format(input())
    ser.write(writer.encode('utf-8'))
    if writer == 'exit':
        break

ser.close()
