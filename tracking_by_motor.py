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


def calc_area(img):
    '''面積計算'''
    img = red_detect(img)
    pix_area = cv2.countNonZero(img)  # ピクセル数
    # パーセントを算出
    h, w = img.shape  # frameの面積
    per = round(100 * float(pix_area) / (w * h), 3)  # 0-100で規格化
    return per


def calc_center(img):
    '''重心座標(x,y)を求める'''
    img = red_detect(img)
    mu = cv2.moments(img, False)
    x, y = int(mu["m10"] / (mu["m00"] + 1e-7)
               ), int(mu["m01"] / (mu["m00"] + 1e-7))
    # 重心を丸でくくる
    # cv2.circle(img, (x, y), 4, 100, 2, 4)
    return x, y


# シリアル通信
def send_serial(params):
    ser.write(params.encode('utf-8'))


cap.start()

MAX_SPEED = 100
r_motor = 100
l_motor = 100

while True:
    ret, frames = cap.read(is_filtered=False)
    color_frame = frames[0]
    depth_frame = frames[1]

    mask = red_detect(color_frame)
    center_pos_x, center_pos_y = calc_center(mask)

    target_distance = cap.depth_frame.get_distance(center_pos_x, center_pos_y)
    error_distance = (center_pos_x - cap.WIDTH / 2) / cap.WIDTH
    r_motor -= error_distance * MAX_SPEED
    l_motor += error_distance * MAX_SPEED

    params = [r_motor, l_motor]
    send_serial(params)

    if target_distance < 0.1:
        break

    time.sleep(0.2)
