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
    # default: 0,0,0,0,0e
    # min: 0,0,0,0,-90e
    # max: 100,100,100,100,90e
    ser.write(params.encode('utf-8'))
    print(f'send: {params}')


CENTER_LINE = 423

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
    vertical_deg = max(min(vertical_pos // 0.0216, 90), -90)  # 角度に変換して上限下限を制限

    depth_pixels = (depth_frame > 0).sum()
    print('Depth value: ', depth_pixels / start_depth_pixels)

    # Depth画像が真っ黒になるまで直進する
    if depth_pixels / start_depth_pixels < 0.1:
        '''ルールベースでつかむ'''
        send_serial(f'7,10,100,0,{vertical_deg}e')  # 距離を詰める
        time.sleep(4)  # 2cm
        print('reached')
        send_serial(f'0,0,0,0,{vertical_deg}e')  # つかむ
        print('grasp')
        time.sleep(2)
        send_serial(f'0,0,0,80,{vertical_deg}e')  # 持ち上げる
        print('bring up')
        time.sleep(2)
        break
    else:
        send_serial(f'7,10,100,0,{vertical_deg}e')  #

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

print('Success')
params = 'e'
send_serial(params)
ser.close()
cap.release()
cv2.destroyAllWindows()
