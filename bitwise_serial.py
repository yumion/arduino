# coding: utf-8
import serial
import time
import sys


ser = serial.Serial(port=sys.argv[1], baudrate=115200)


# シリアル通信
def send_serial(motor, value, isreading=False):
    '''シリアル通信'''
    send = motor * 32 + value  # 8bitを10進数で表記
    send = send.to_bytes(1, 'big')  # byteに変換(ASCIIで表示される)
    ser.write(send)
    print(f"send: {send}, int: {int.from_bytes(send, 'big')}, bit: {format(int.from_bytes(send, 'big'), '08b')}")
    # read
    if isreading:
        while ser.inWaiting() > 0:
            read = ser.readline()
            read = read.strip().decode('utf-8')
            print(read)


r_motor = 0
l_motor = 0
grasp = 0
vertical = 0
horizontal = 9

time.sleep(5)

while True:
    if r_motor < 20:
        r_motor += 1
        l_motor += 1
    else:
        r_motor = 0
        l_motor = 0

    if grasp == 0:
        grasp = 1
        vertical = 1
        horizontal = 1
    else:
        grasp = 0
        vertical = 0
        horizontal = 18

    params = [r_motor, l_motor, grasp, vertical, horizontal]
    for i, param in enumerate(params):
        send_serial(i, param)
    send_serial(5, 0, True)  # decide dc motor
    print('-----------------')
    time.sleep(0.2)


ser.close()
