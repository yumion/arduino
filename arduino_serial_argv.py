
import serial
from time import sleep
import sys


'''sys.argv
[1] : port(COM3, /dev/ttyUSB0, /dev/tty.cu.usb***)
[2] : 80,170,60,90,0,100e
'''

ser = serial.Serial(port=sys.argv[1], baudrate=9600)
sleep(5)

input_params = sys.argv[2]
writer = input_params
for _ in range(5):
    ser.write(writer.encode('utf-8'))
    print(writer)

'''
while ser.inWaiting() > 0:
    read = ser.readline()
    read = read.strip().decode('utf-8')
    print(read)
'''
ser.close()
