# coding: utf-8
import cv2
import numpy as np
import serial
import time
import sys
from realsensecv import RealsenseCapture


ser = serial.Serial(port=sys.argv[1], baudrate=9600)
cap = RealsenseCapture()


def red_detect(img):
    '''赤色のマスク生成'''
    # HSV色空間に変換
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    # 赤色のHSVの値域1
    hsv_min = np.array([0, 127, 0])
    hsv_max = np.array([149, 255, 255])
    mask1 = cv2.inRange(hsv, hsv_min, hsv_max)
    # 赤色のHSVの値域2
    hsv_min = np.array([150, 127, 0])
    hsv_max = np.array([179, 255, 255])
    mask2 = cv2.inRange(hsv, hsv_min, hsv_max)
    return mask1 + mask2


def green_detect(img):
    '''緑色のマスク生成'''
    # HSV色空間に変換
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    # 緑色のHSVの値域
    hsv_min = np.array([30, 64, 0])
    hsv_max = np.array([90, 255, 255])
    mask = cv2.inRange(hsv, hsv_min, hsv_max)
    return mask


def calc_center(img):
    '''重心座標(x,y)を求める'''
    mu = cv2.moments(img, False)
    x, y = int(mu["m10"] / (mu["m00"] + 1e-7)), int(mu["m01"] / (mu["m00"] + 1e-7))
    return x, y


def send_serial(params):
    '''シリアル通信'''
    ser.write(params.encode('utf-8'))
    print(f'send: {params}')


cap.start()
time.sleep(5)

ret, frames = cap.read(is_filtered=False)
start_rgb = frames[0]
start_depth = frames[1]
start_depth_pixels = (start_depth > 0).sum()

while True:
    ret, frames = cap.read(is_filtered=False)
    color_frame = frames[0]
    depth_frame = frames[1]

    images = np.hstack((color_frame, depth_frame))
    cv2.imshow('RealSense', images)
    if cv2.waitKey(200) & 0xFF == ord('q'):
        break

    depth_pixels = (depth_frame > 0).sum()
    print('Depth value: ', depth_pixels)

    params = '6,10,180,0,90e'
    send_serial(params)

    if depth_pixels / start_depth_pixels < 0.1:
        '''ルールベースでつかむ'''
        send_serial('6,10,180,0,90e')  # 距離を詰める
        time.sleep(4)
        send_serial('0,0,0,0,90e')  # つかむ
        time.sleep(2)
        send_serial('0,0,0,90,90e')  # 持ち上げる
        break

ret, frames = cap.read(is_filtered=False)
color_frame = frames[0]
depth_frame = frames[1]
end_depth_pixels = (depth_frame > 0).sum()
count = 0

while True:
    time.sleep(1)
    ret, frames = cap.read(is_filtered=False)
    color_frame = frames[0]
    depth_frame = frames[1]
    depth_pixels = (depth_frame > 0).sum()

    if depth_pixels / end_depth_pixels < 0.1:
        print('Failed')
        break
    elif count == 10:
        print('Success')
        break
    count += 1


params = '0,0,0,180,90e'
send_serial(params)
ser.close()
cap.release()
cv2.destroyAllWindows()
