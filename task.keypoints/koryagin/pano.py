import cv2
import numpy as np
import scipy
from scipy import ndimage
import argparse
from os import listdir
from os.path import isfile, join


def readDir(args):
	source_files = [f for f in listdir(args.source_directory) if isfile(
		join(args.source_directory, f))]
	images = []
	for source_name in source_files:
		source_path = args.source_directory + source_name
		print source_path
		img = cv2.imread(source_path)
		images.append(cv2.cvtColor(img, cv2.COLOR_BGR2GRAY))
	return images


def createParser():
	parser = argparse.ArgumentParser(add_help=True)
	parser.add_argument('-s', '--source_directory', required=True)
	parser.add_argument('-r', '--result', nargs='?', default="result.jpg")
	return parser


def show(img):
	cv2.imshow("image", img)
	cv2.waitKey()


def pano(images):
	sift = cv2.SIFT()
	key_points = []
	img_des = []
	for image in images:
		kp, d = sift.detectAndCompute(image, None)
		print len(kp)
		key_points.append(kp)
		img_des.append(d)
	distances = [[[[None for m in range(len(img_des[j]))] for n in range(
		len(img_des[j]))] for j in range(len(img_des))] for i in range(len(img_des))]
	means = [[None for j in range(len(img_des))] for i in range(len(img_des))]
	same_points = [[[]
					for j in range(len(img_des))] for i in range(len(img_des))]
	# print img_des

	for i in range(len(img_des)):
		for j in range(len(img_des)):
			if i < j:
				mean = 0
				for n in range(len(img_des[i])):
					for m in range(len(img_des[j])):
						dist = np.linalg.norm(img_des[i][n] - img_des[j][m])
						mean = mean + dist
						distances[i][j][n][m] = dist
				means[i][j] = mean / (len(img_des[i]) * len(img_des[j]))
				print means[i][j]

	for i in range(len(img_des)):
		for j in range(len(img_des)):
			if i < j:
				for n in range(len(img_des[i])):
					for m in range(len(img_des[j])):
						if distances[i][j][n][m] < 50:
							same_points[i][j].append([n, m])

	for i in range(len(img_des)):
		for j in range(len(img_des)):
			if i < j:
				print same_points[i][j]
				images[i] = cv2.drawKeypoints(images[i], [key_points[i][k] for  k in [
											  same_points[i][j][q][0] for q in range(len(same_points[i][j]))]])
				images[j] = cv2.drawKeypoints(images[j], [key_points[j][k] for  k in [
											  same_points[i][j][q][1] for q in range(len(same_points[i][j]))]])
	for i in range(len(images)):
		# cv2.imshow("image " + str(i), images[i])
		cv2.imwrite("image" + str(i) + ".jpg", images[i])


parser = createParser()
args = parser.parse_args()

images = readDir(args)

# for item in images:
# 	show(item)

pano(images)
# show(src)
