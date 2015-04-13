#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <vector>
#include <utility>

using namespace std;
using namespace cv;

void show_image(Mat const& src) {
  // cv::imwrite(name, src);
  Mat dst;
  cv::resize(src, dst, Size(src.cols / 3, src.rows / 3));
  cv::imshow("image", dst);
  cv::waitKey();
  cv::destroyWindow("image");
}

void printVect(string name, vector<pair<Point2i, Point2i> > vect) {
  cout << name << endl;
  for (int i = 0; i < vect.size(); ++i) {
    cout << "[[" << vect[i].first.x << ", " << vect[i].first.y << "], [" << vect[i].second.x << ", " << vect[i].second.y << "]]" << endl;
  }
}
void printRects(pair<Point2i, Point2i> rect1, pair<Point2i, Point2i> rect2) {
  cout << "introsecting" << endl;
  cout << "[[" << rect1.first.x << ", " << rect1.first.y << "], [" << rect1.second.x << ", " << rect1.second.y << "]]" << endl;
  cout << "[[" << rect2.first.x << ", " << rect2.first.y << "], [" << rect2.second.x << ", " << rect2.second.y << "]]" << endl;
}

vector<vector<pair<Point2i, Point2i> > > readTouristLocations(string sourceFileName, int n) {
  ifstream toursitLocationsFile;
  toursitLocationsFile.open(sourceFileName);
  vector<vector<pair<Point2i, Point2i> > > toursitLocations;
  if (!toursitLocationsFile.is_open()) {
    cout << "Can not get tourist locations" << endl;
    return toursitLocations;
  }

  for (int i = 0; i < n; ++i) {
    vector<pair<Point2i, Point2i> > curLocations;
    int numberOfRects;
    toursitLocationsFile >> numberOfRects;
    for (int j = 0; j < numberOfRects; ++j) {
      int x1, x2, y1, y2;
      toursitLocationsFile >> x1 >> y1 >> x2 >> y2;
      curLocations.push_back(make_pair(Point2i(x1 * 4, y1 * 4), Point2i(x2 * 4, y2 * 4)));
    }
    toursitLocations.push_back(curLocations);
  }

  return toursitLocations;
}

vector<pair<Point2i, Point2i> > transformTouristLocations(vector<pair<Point2i, Point2i> > origin, Mat homography) {
  // printVect("origin", origin);
  vector<pair<Point2i, Point2i> > newLocations;
  for (int i = 0; i < origin.size(); ++i) {
    pair<Point2i, Point2i> curPair = origin[i];
    vector<Point2f> pointsVector;
    pointsVector.push_back(origin[i].first);
    pointsVector.push_back(cvPoint(origin[i].second.x, origin[i].first.y));
    pointsVector.push_back(cvPoint(origin[i].first.x, origin[i].second.y));
    pointsVector.push_back(origin[i].second);
    vector<Point2f> outputPoints(4);
    perspectiveTransform(pointsVector, outputPoints, homography);

    Point2i pUL = cvPoint(static_cast<int>(outputPoints[0].x), static_cast<int>(outputPoints[0].y));
    Point2i pLR = pUL;
    for (int k = 1; k < outputPoints.size(); ++k) {
      if (pUL.x > static_cast<int>(outputPoints[k].x)) pUL.x = static_cast<int>(outputPoints[k].x);
      if (pUL.y > static_cast<int>(outputPoints[k].y)) pUL.y = static_cast<int>(outputPoints[k].y);
      if (pLR.x < static_cast<int>(outputPoints[k].x)) pLR.x = static_cast<int>(outputPoints[k].x);
      if (pLR.y < static_cast<int>(outputPoints[k].y)) pLR.y = static_cast<int>(outputPoints[k].y);
    }
    newLocations.push_back(make_pair(pUL, pLR));
  }
  // printVect("newLocations", newLocations);
  return newLocations;
}

bool introsect(pair<Point2i, Point2i> rect1, pair<Point2i, Point2i> rect2) {
  if (rect1.first.x > rect2.second.x || rect1.second.x < rect2.first.x ||
    rect1.first.y > rect2.second.y || rect1.second.y < rect2.first.y) {
    return false;
  } else {
    return true;
  }
}

int touristlessization(string inputFolderName, int n, int objectId, string outputFolderName, vector<vector<pair<Point2i, Point2i> > > touristLocations) {
  Mat imgObject = imread(inputFolderName + to_string(objectId) + ".jpg", CV_LOAD_IMAGE_GRAYSCALE);
  Mat imgObjectCol = imread(inputFolderName + to_string(objectId) + ".jpg");

  SiftFeatureDetector detector(0, 4, 0.04, 10, 1.6);
  SiftDescriptorExtractor extractor(3.0);

  std::vector<KeyPoint> keypointsOfObject;
  detector.detect(imgObject, keypointsOfObject);

  Mat descriptorsOfObject;
  extractor.compute(imgObject, keypointsOfObject, descriptorsOfObject);
  vector<vector<pair<Point2i, Point2i> > > newTouristLocations(n);
  newTouristLocations[objectId - 1] = touristLocations[objectId - 1];

  if (descriptorsOfObject.empty()) {
    cout << "object descriptors error\n";
    return -1;
  }

  for (int i = 1; i <= n; ++i) {
    if (i == objectId) {
      continue;
    }
    std::vector<KeyPoint> keypointsOfScene;
    Mat descriptorsOfScene;

    Mat imgScene = imread(inputFolderName + to_string(i) + ".jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat imgSceneCol = imread(inputFolderName + to_string(i) + ".jpg");

    detector.detect(imgScene, keypointsOfScene);
    extractor.compute(imgScene, keypointsOfScene, descriptorsOfScene);

    if (descriptorsOfScene.empty()) {
      cout << "scene descriptors error\n";
      return -1;
    }

    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptorsOfObject, descriptorsOfScene, matches);

    double maxDist = 0;
    double minDist = 100;

    for (int j = 0; j < descriptorsOfObject.rows; ++j) {
      double dist = matches[j].distance;
      if (dist < minDist)
        minDist = dist;
      if (dist > maxDist)
        maxDist = dist;
    }

    vector<DMatch> bestMatches;
    for (int j = 0; j < descriptorsOfObject.rows; ++j) {
      if (matches[j].distance < 3 * minDist) {
        bestMatches.push_back(matches[j]);
      }
    }

    vector<Point2f> obj;
    vector<Point2f> scene;
    for (int j = 0; j < bestMatches.size(); ++j) {
      obj.push_back(keypointsOfObject[bestMatches[j].queryIdx].pt);
      scene.push_back(keypointsOfScene[bestMatches[j].trainIdx].pt);
    }

    Mat homography = findHomography(obj, scene, CV_RANSAC);
    newTouristLocations[i - 1] = transformTouristLocations(touristLocations[i - 1], homography);

    warpPerspective(imgSceneCol, imgSceneCol, homography.inv(), imgObject.size());
    imwrite(outputFolderName + to_string(i) + ".jpg", imgSceneCol);
    cout << "Image number " << i << " done\n";
  }
  imwrite(outputFolderName + to_string(objectId) + ".jpg", imgObjectCol);

  vector<Mat> imagesNormalizedCol;
  for (int i = 1; i <= n; ++i) {
    Mat imageCol = imread(outputFolderName + to_string(i) + ".jpg");
    imagesNormalizedCol.push_back(imageCol);
  }

  for (int i = 0; i < newTouristLocations[objectId - 1].size(); ++i) {
    // пройти по всем картинкам - если на одной из них в данном прямоугольнике нету туристов, то просто вырезаем из неё кусок
    bool imgFound = false;
    int curimg = 0;
    while (!imgFound && curimg < n) {
      if (curimg != objectId - 1) {
        int j = 0;
        bool introsecting = false;
        while (!introsecting && j < newTouristLocations[curimg].size()) {
          if (introsect(newTouristLocations[objectId - 1][i], newTouristLocations[curimg][j])) {
            // printRects(newTouristLocations[objectId - 1][i], newTouristLocations[curimg][j]);
            introsecting = true;
          }
          ++j;
        }
        if (!introsecting) {
          imgFound = true;
        } else {
          ++curimg;
        }
      } else {
        ++curimg;
      }

    }
    if (!imgFound) {
      cout << "Can not found image to replace tourists number " << i << endl;
      // если нету такого прямоугольника, то надо запускать какой-то рекурсивный алгоритм, где мы вырезаем кусок из этого прямоугольника
      // а потом делаем то же самое для того что получилось по рекурсии, при этом говорим, что одно из изображений мы уже использовали
    } else {
      cout << "Replacing tourists number " << i << " with " << curimg + 1 << ".jpg" << endl;
      int left = newTouristLocations[objectId - 1][i].first.x;
      int top = newTouristLocations[objectId - 1][i].first.y;
      int width = newTouristLocations[objectId - 1][i].second.x - newTouristLocations[objectId - 1][i].first.x;
      int height = newTouristLocations[objectId - 1][i].second.y - newTouristLocations[objectId - 1][i].first.y;
      Mat a = imagesNormalizedCol[curimg](Rect(left, top, width, height));
      a.copyTo(imgObjectCol(Rect(left, top, width, height)));
    }
  }
  imwrite(outputFolderName + "touristless" + to_string(objectId) + ".jpg", imgObjectCol);
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc < 4 || argc > 5) {
    cout << "Usage: path to folder with images, number of images, number of the best photo, [output folder]\n";
    return -1;
  }
  string inputFolderName = argv[1]; //"../testdata/occlusion_01/images/";
  int n = stoi(argv[2]);
  int objectId = stoi(argv[3]);
  string outputFolderName = (argc == 5 ? argv[4] : "../testdata/occlusion_01/images/output/");
  string toursitLocationsFileName = "../testdata/occlusion_01/coordinates.txt";

  if (n < 2) {
    cout << "Need more images\n";
    return - 1;
  }
  vector<vector<pair<Point2i, Point2i> > > touristLocations = readTouristLocations(toursitLocationsFileName, n);
  if (touristLocations.size() > 0) {
    touristlessization(inputFolderName, n, objectId, outputFolderName, touristLocations);
  } else {
    cout << "Can not read tourist locations" << endl;
  }
  return 0;
}