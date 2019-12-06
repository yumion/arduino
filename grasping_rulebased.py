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


MAX_SPEED = 100
GOAL_POS = 423

while True:
    ret, frames = cap.read(is_filtered=False)
    color_frame = frames[0]
    depth_frame = frames[1]

    mask = red_detect(color_frame.copy())
    mask_pixels = (mask > 0).sum()
    print('Area: ', mask_pixels / (mask.shape[0] * mask.shape[1]))
    center_pos_x, center_pos_y = calc_center(mask)
    print(f'G({center_pos_x}, {center_pos_y})')

    target_distance = cap.depth_frame.get_distance(center_pos_x, center_pos_y)
    error_distance = (center_pos_x - GOAL_POS) / GOAL_POS
    print(f'error: {error_distance}  |   target distance: {target_distance}')

    params = f'10,10,180,0,90e'
    send_serial(params)

    mask_RGB = cv2.cvtColor(mask, cv2.COLOR_GRAY2RGB)
    cv2.circle(mask_RGB, (center_pos_x, center_pos_y), 5, (0, 0, 255), thickness=-1)
    images = np.hstack((color_frame, mask_RGB))
    cv2.imshow('RGB', images)

    if cv2.waitKey(200) & 0xFF == ord('q'):
        break

    # time.sleep(0.2)

params = '0,0,0,180,90e'
send_serial(params)
ser.close()
cap.release()
cv2.destroyAllWindows()
