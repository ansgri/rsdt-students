import numpy as np
import cv2
from scipy import ndimage
import argparse


def createParser():
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument('-s', '--source', required=True)
    parser.add_argument(
        '-c', '--contrast', nargs='?', default="contrast.jpg")
    parser.add_argument('-r', '--rotated', nargs='?', default="rotated.jpg")
    return parser


def local_auto_contrast(img):
    shape = cv2.getStructuringElement(0, (200, 200))
    local_minimum = cv2.morphologyEx(img, cv2.MORPH_ERODE, shape)
    local_maximum = cv2.morphologyEx(img, cv2.MORPH_DILATE, shape)
    local_minimum = np.float32(local_minimum)
    local_maximum = np.float32(local_maximum)
    img = np.float32(img)
    res = cv2.divide(cv2.subtract(img, local_minimum), cv2.subtract(local_maximum, local_minimum))
    res *= 255
    res = cv2.threshold(res, 200, 255, 2)[1]
    return res


def rotate(img):
    h, w = img.shape[:2]
    roi = img[h / 2 - 500:h / 2, w / 2 - 500:w / 2]
    shape = cv2.getStructuringElement(0, (5, 100))
    cy, cx = roi.shape[:2]
    angle = 0
    max_average = 0
    flag = True
    for i in range(-45, 45, 1):
        M = cv2.getRotationMatrix2D((cx / 2, cy / 2), i, scale=3)
        rotated_img = cv2.warpAffine(roi, M, (cx, cy))
        sobelx = cv2.Sobel(rotated_img, cv2.CV_16S, 0, 1, scale=1, delta=0, ksize=3, borderType=cv2.BORDER_DEFAULT)
        morph = cv2.morphologyEx(sobelx, cv2.MORPH_DILATE, shape)
        mean = cv2.mean(morph)[0]
        if flag or mean > max_average:
            flag = False
            angle = i
            max_average = mean
    return angle


parser = createParser()
args = parser.parse_args()

src = cv2.imread(args.source)
src = cv2.cvtColor(src, cv2.CV_LOAD_IMAGE_GRAYSCALE)

binarized = local_auto_contrast(src)
cv2.imwrite(args.binarized, binarized)
rotated = rotate(src)
cv2.imwrite(args.rotated, rotated)
