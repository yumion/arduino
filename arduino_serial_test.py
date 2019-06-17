import serial
from time import sleep

ser = serial.Serial(port='/dev/cu.usbserial-A1065SGP', baudrate=9600 )

writer = u'{0}'.format(input())
ser.write(writer.encode('utf-8'))

while ser.inWaiting() > 0:
    read = ser.readline()
    read = read.strip().decode('utf-8')
    print(read)
# sleep(1)


ser.close()
