import cv2
import numpy as np
import scipy
from scipy import ndimage
import argparse

def createParser():
	parser = argparse.ArgumentParser(add_help = True)
	parser.add_argument('-s', '--source', required=True)
	parser.add_argument('-b', '--binarized', nargs='?', default="binarized.jpg")
	parser.add_argument('-r', '--rotated', nargs='?', default="rotated.jpg")
	return parser

def binarize(src):
	BLUR_APERTURE = 3;
	MORPH_APERTURE = 41;

	src = cv2.GaussianBlur(src, (BLUR_APERTURE, BLUR_APERTURE), 0)
	strel = cv2.getStructuringElement(cv2.MORPH_CROSS, (MORPH_APERTURE, MORPH_APERTURE))
	dst = cv2.morphologyEx(src, cv2.MORPH_CLOSE, strel)
	dst = 255 - (dst - src);
	dst = cv2.threshold(dst, 238, 255, cv2.THRESH_BINARY)[1]
	return dst

def morph_mean(src, angle): 
	dst = ndimage.rotate(src, angle)
	strel = cv2.getStructuringElement(cv2.MORPH_RECT, (1, 16))
	dst = cv2.morphologyEx(dst, cv2.MORPH_OPEN, strel)
	mean = cv2.mean(dst)[0]
	print mean
	return mean

def smart_rotation(src):
	angle_from = -30
	angle_to = 30
	angle_step = 5
	sobelx = cv2.Sobel(src, cv2.CV_8U, 1, 0, ksize = 3)
	max_mean = {"angle":angle_from, "mean":morph_mean(sobelx, angle_from)}
	for angle in xrange(angle_from + angle_step, angle_to, angle_step):
		mean = morph_mean(sobelx, angle)
		if mean > max_mean["mean"]:
			max_mean["angle"] = angle
			max_mean["mean"] = mean
	return ndimage.rotate(src, max_mean["mean"])


parser = createParser()
args = parser.parse_args()

src = cv2.imread(args.source)
src = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)

binarized = binarize(src)
cv2.imwrite(args.binarized, binarized)
rotated = smart_rotation(src)
cv2.imwrite(args.rotated, rotated)

