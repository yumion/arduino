import serial

ser = serial.Serial("/dev/cu.usbserial-A1065SGP")
#use your Arduino's serial port name instead of "/dev/cu.usbmodem1421".
print("if you input 'go', serial starts.")
c = input()
if c == 'go':
    print("::serial start::")
    ser.write(u'start'.encode('utf-8'))
x = 0
y = 0
z = 0

while True:
    if ser.inWaiting() > 0:
        if x == 4:
            x = 1
            y = 1
            z = 1
        print(ser.inWaiting())
        print(ser.readline())
        str = '{0},{1},{2}='.format(x,y,z)
        ser.write(str)
        x = x + 1
        y = y + 1
        z = z + 1
