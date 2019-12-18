# coding: utf-8
import cv2
import numpy as np
import serial
import time
import sys
import os

from realsensecv import RealsenseCapture


os.system(f'sudo chmod 666 {sys.argv[1]}')
ser = serial.Serial(port=sys.argv[1], baudrate=115200)
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


CENTER_LINE = 423

cap.start()
time.sleep(5)

# default
params = [0, 0, 1, 0, 9]
for i, param in enumerate(params):
    send_serial(i, param, True)

ret, frames = cap.read(is_filtered=False)
start_rgb = frames[0]
start_depth = frames[1]
start_depth_pixels = (start_depth > 0).sum()

while True:
    ret, frames = cap.read(is_filtered=False)
    color_frame = frames[0]
    depth_frame = frames[1]

    mask = green_detect(color_frame.copy())
    center_pos_x, center_pos_y = calc_center(mask)
    # print(f'G({center_pos_x}, {center_pos_y})')
    target_distance = cap.depth_frame.get_distance(center_pos_x, center_pos_y)

    cv2.circle(color_frame, (center_pos_x, center_pos_y), 5, (0, 0, 255), thickness=-1)
    cv2.line(color_frame, (CENTER_LINE, 0), (CENTER_LINE, cap.HEGIHT), (255, 0, 0))
    images = np.hstack((color_frame, depth_frame))
    cv2.imshow('RealSense', images)
    if cv2.waitKey(200) & 0xFF == ord('q'):
        break

    vertical_pos = (0.0016 * target_distance * 100 - 0.0004) * (center_pos_x - CENTER_LINE)  # ピクセル間距離(cm)
    # print('vertical position: ', vertical_pos)
    vertical_deg = (max(min(vertical_pos // 0.0216, 90), -90) + 90) // 10  # 角度に変換して上限下限を制限して-90~90を0~18に変換

    depth_pixels = (depth_frame > 0).sum()
    print('Depth value: ', depth_pixels / start_depth_pixels)

    # Depth画像が真っ黒になるまで直進する
    if depth_pixels / start_depth_pixels < 0.1:
        '''ルールベースでつかむ'''
        params = [2, 2]  # 距離を詰める
        for i, param in enumerate(params):
            send_serial(i, param, True)
        time.sleep(4)  # 2cm
        print('reached')

        params = [0, 0, 0, 0]  # つかむ
        for i, param in enumerate(params):
            send_serial(i, param, True)
        print('grasp')
        time.sleep(2)

        send_serial(3, 1, True)  # 持ち上げる
        print('bring up')
        time.sleep(2)
        break
    else:
        params = [2, 2, 1, 0, int(vertical_deg)]
        for i, param in enumerate(params):
            send_serial(i, param, True)


for i in range(5):
    ret, frames = cap.read(is_filtered=False)
    color_frame = frames[0]
    depth_frame = frames[1]
    images = np.hstack((color_frame, depth_frame))
    cv2.imshow('RealSense', images)
    if cv2.waitKey(1000) & 0xFF == ord('q'):
        break

    depth_pixels = (depth_frame > 0).sum()
    print(f'count: {i} | {depth_pixels / start_depth_pixels}')

    if depth_pixels / start_depth_pixels < 0.5:
        # デプス画像の視界が開けなければ把持失敗
        print('Failed')
        break
    if i == 4:
        print('Success!')
        send_serial(3, 0, True)
        time.sleep(3)

params = [0, 0, 1, 0, 9]
for i, param in enumerate(params):
    send_serial(i, param, True)

ser.close()
cap.release()
cv2.destroyAllWindows()
