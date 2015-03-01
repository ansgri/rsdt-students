import cv2
import numpy as np
import scipy
from scipy import ndimage
import argparse

BLUR_APERTURE = 3
MORPH_APERTURE = 40
MORPH_APERTURE_WIDTH = 3
MORPH_APERTURE_HEIGHT = 60
INITIAL_ANGLE = -360

def binarization(src, args_bin):
  src_blured = cv2.GaussianBlur(src, (BLUR_APERTURE, BLUR_APERTURE), 0)
  struct_element = cv2.getStructuringElement(cv2.MORPH_CROSS, (MORPH_APERTURE, MORPH_APERTURE))
  src_ex = cv2.morphologyEx(src_blured, cv2.MORPH_CLOSE, struct_element)
  reverse = 255 - (src_ex - src_blured)
  result = cv2.threshold(reverse, 238, 255, cv2.THRESH_BINARY)[1]
  cv2.imwrite(args_bin, result)

def rotation(src, args_rot):
  src_blur = cv2.GaussianBlur(src, (BLUR_APERTURE, BLUR_APERTURE), 0)
  src_grad = cv2.Sobel(src, cv2.CV_8U, 1, 0, ksize = 3)
  max_angle = INITIAL_ANGLE
  maximum = 0
  deviation = 30
  step = 1

  for angle in range(360 - deviation, 360 + deviation, step):
    print 'processing with angle = ', angle
    temp = ndimage.rotate(src_grad, angle % 360)
    struct_element = cv2.getStructuringElement(cv2.MORPH_RECT, (MORPH_APERTURE_WIDTH, MORPH_APERTURE_HEIGHT))
    temp_open = cv2.morphologyEx(temp, cv2.MORPH_OPEN, struct_element)
    mean = cv2.mean(temp_open)[0]
    if (mean > maximum or max_angle == INITIAL_ANGLE):
      max_angle = angle % 360
      maximum = mean

  result = ndimage.rotate(src, max_angle)
  cv2.imwrite(args_rot, result)


def main():
    parser = argparse.ArgumentParser(add_help = True)
    parser.add_argument('-s', '--source', default="paper_lined.jpg")
    parser.add_argument('-b', '--binarized', nargs='?', default="binarized.jpg")
    parser.add_argument('-r', '--rotated', nargs='?', default="rotated.jpg")

    args = parser.parse_args()
    src = cv2.imread(args.source)
    src = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)

    binarization(src, args.binarized)
    rotation(src, args.rotated)

if __name__ == "__main__":
    main()