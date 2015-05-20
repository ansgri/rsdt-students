#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <vector>
#include <utility>
#include <dirent.h>

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

void print_vect(string name, vector<pair<Point2i, Point2i> > vect) {
  cout << name << endl;
  for (int i = 0; i < vect.size(); ++i) {
    cout << "[[" << vect[i].first.x << ", " << vect[i].first.y << "], [" << vect[i].second.x << ", " << vect[i].second.y << "]]" << endl;
  }
}
void print_rects(pair<Point2i, Point2i> rect1, pair<Point2i, Point2i> rect2) {
  cout << "introsecting" << endl;
  cout << "[[" << rect1.first.x << ", " << rect1.first.y << "], [" << rect1.second.x << ", " << rect1.second.y << "]]" << endl;
  cout << "[[" << rect2.first.x << ", " << rect2.first.y << "], [" << rect2.second.x << ", " << rect2.second.y << "]]" << endl;
}

vector<vector<pair<Point2i, Point2i> > > read_tourist_locations(string source_file_name, int n) {
  ifstream toursit_locations_file;
  toursit_locations_file.open(source_file_name);
  vector<vector<pair<Point2i, Point2i> > > toursit_locations;
  if (!toursit_locations_file.is_open()) {
    cout << "Can not get tourist locations" << endl;
    return toursit_locations;
  }

  for (int i = 0; i < n; ++i) {
    vector<pair<Point2i, Point2i> > cur_locations;
    int number_of_rects;
    toursit_locations_file >> number_of_rects;
    for (int j = 0; j < number_of_rects; ++j) {
      int x1, x2, y1, y2;
      toursit_locations_file >> x1 >> y1 >> x2 >> y2;
      cur_locations.push_back(make_pair(Point2i(x1 * 4, y1 * 4), Point2i(x2 * 4, y2 * 4)));
    }
    toursit_locations.push_back(cur_locations);
  }

  return toursit_locations;
}

vector<pair<Point2i, Point2i> > transform_tourist_locations(vector<pair<Point2i, Point2i> > origin, Mat homography) {
  // print_vect("origin", origin);
  vector<pair<Point2i, Point2i> > new_locations;
  for (int i = 0; i < origin.size(); ++i) {
    pair<Point2i, Point2i> curPair = origin[i];
    vector<Point2f> points_vector;
    points_vector.push_back(origin[i].first);
    points_vector.push_back(cvPoint(origin[i].second.x, origin[i].first.y));
    points_vector.push_back(cvPoint(origin[i].first.x, origin[i].second.y));
    points_vector.push_back(origin[i].second);
    vector<Point2f> output_points(4);
    perspectiveTransform(points_vector, output_points, homography);

    Point2i pUL = cvPoint(static_cast<int>(output_points[0].x), static_cast<int>(output_points[0].y));
    Point2i pLR = pUL;
    for (int k = 1; k < output_points.size(); ++k) {
      if (pUL.x > static_cast<int>(output_points[k].x)) pUL.x = static_cast<int>(output_points[k].x);
      if (pUL.y > static_cast<int>(output_points[k].y)) pUL.y = static_cast<int>(output_points[k].y);
      if (pLR.x < static_cast<int>(output_points[k].x)) pLR.x = static_cast<int>(output_points[k].x);
      if (pLR.y < static_cast<int>(output_points[k].y)) pLR.y = static_cast<int>(output_points[k].y);
    }
    new_locations.push_back(make_pair(pUL, pLR));
  }
  // print_vect("new_locations", new_locations);
  return new_locations;
}

bool introsect(pair<Point2i, Point2i> rect1, pair<Point2i, Point2i> rect2) {
  if (rect1.first.x > rect2.second.x || rect1.second.x < rect2.first.x ||
    rect1.first.y > rect2.second.y || rect1.second.y < rect2.first.y) {
    return false;
  } else {
    return true;
  }
}

int get_number_of_files(const char *path) {
  int n = 0;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path)) != NULL) {
      while ((ent = readdir(dir)) != NULL) {
          if (ent->d_name[0] != '.') {
              ++n;
              // cv::Mat img = cv::imread(std::string(folder) + ent->d_name, CV_LOAD_IMAGE_GRAYSCALE);
              // vect.push_back(std::make_pair(label, img));
          }
      }
      closedir (dir);
  } else {
      std::cout << "Can not open directory " << path << std::endl;
  }
  return n;
}

int touristlessization(string input_folder_name, int n, int object_id, string output_folder_name, vector<vector<pair<Point2i, Point2i> > > tourist_locations) {
  Mat img_object = imread(input_folder_name + to_string(object_id) + ".jpg", CV_LOAD_IMAGE_GRAYSCALE);
  Mat img_object_col = imread(input_folder_name + to_string(object_id) + ".jpg");

  SiftFeatureDetector detector(0, 4, 0.04, 10, 1.6);
  SiftDescriptorExtractor extractor(3.0);

  std::vector<KeyPoint> keypoints_of_object;
  detector.detect(img_object, keypoints_of_object);

  Mat descriptors_of_object;
  extractor.compute(img_object, keypoints_of_object, descriptors_of_object);
  vector<vector<pair<Point2i, Point2i> > > new_tourist_locations(n);
  new_tourist_locations[object_id - 1] = tourist_locations[object_id - 1];

  if (descriptors_of_object.empty()) {
    cout << "object descriptors error\n";
    return -1;
  }

  for (int i = 1; i <= n; ++i) {
    if (i == object_id) {
      continue;
    }
    std::vector<KeyPoint> keypoints_of_scene;
    Mat descriptors_of_scene;

    Mat img_scene = imread(input_folder_name + to_string(i) + ".jpg", CV_LOAD_IMAGE_GRAYSCALE);
    Mat img_scene_col = imread(input_folder_name + to_string(i) + ".jpg");

    detector.detect(img_scene, keypoints_of_scene);
    extractor.compute(img_scene, keypoints_of_scene, descriptors_of_scene);

    if (descriptors_of_scene.empty()) {
      cout << "scene descriptors error\n";
      return -1;
    }

    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors_of_object, descriptors_of_scene, matches);

    double max_dist = 0;
    double min_dist = 100;

    for (int j = 0; j < descriptors_of_object.rows; ++j) {
      double dist = matches[j].distance;
      if (dist < min_dist)
        min_dist = dist;
      if (dist > max_dist)
        max_dist = dist;
    }

    vector<DMatch> best_matches;
    for (int j = 0; j < descriptors_of_object.rows; ++j) {
      if (matches[j].distance < 3 * min_dist) {
        best_matches.push_back(matches[j]);
      }
    }

    vector<Point2f> obj;
    vector<Point2f> scene;
    for (int j = 0; j < best_matches.size(); ++j) {
      obj.push_back(keypoints_of_object[best_matches[j].queryIdx].pt);
      scene.push_back(keypoints_of_scene[best_matches[j].trainIdx].pt);
    }

    Mat homography = findHomography(obj, scene, CV_RANSAC);
    new_tourist_locations[i - 1] = transform_tourist_locations(tourist_locations[i - 1], homography);

    warpPerspective(img_scene_col, img_scene_col, homography.inv(), img_object.size());
    imwrite(output_folder_name + to_string(i) + ".jpg", img_scene_col);
    cout << "Image number " << i << " done\n";
  }
  imwrite(output_folder_name + to_string(object_id) + ".jpg", img_object_col);

  vector<Mat> images_normalized_col;
  for (int i = 1; i <= n; ++i) {
    Mat image_col = imread(input_folder_name + to_string(i) + ".jpg");
    images_normalized_col.push_back(image_col);
  }

  for (int i = 0; i < new_tourist_locations[object_id - 1].size(); ++i) {
    // пройти по всем картинкам - если на одной из них в данном прямоугольнике нету туристов, то просто вырезаем из неё кусок
    bool img_found = false;
    int curimg = 0;
    while (!img_found && curimg < n) {
      if (curimg != object_id - 1) {
        int j = 0;
        bool introsecting = false;
        while (!introsecting && j < new_tourist_locations[curimg].size()) {
          if (introsect(new_tourist_locations[object_id - 1][i], new_tourist_locations[curimg][j])) {
            // print_rects(new_tourist_locations[object_id - 1][i], new_tourist_locations[curimg][j]);
            introsecting = true;
          }
          ++j;
        }
        if (!introsecting) {
          img_found = true;
        } else {
          ++curimg;
        }
      } else {
        ++curimg;
      }

    }
    if (!img_found) {
      cout << "Can not found image to replace tourists number " << i << endl;
      // если нету такого прямоугольника, то надо запускать какой-то рекурсивный алгоритм, где мы вырезаем кусок из этого прямоугольника
      // а потом делаем то же самое для того что получилось по рекурсии, при этом говорим, что одно из изображений мы уже использовали
    } else {
      cout << "Replacing tourists number " << i << " with " << curimg + 1 << ".jpg" << endl;
      int left = new_tourist_locations[object_id - 1][i].first.x;
      int top = new_tourist_locations[object_id - 1][i].first.y;
      int width = new_tourist_locations[object_id - 1][i].second.x - new_tourist_locations[object_id - 1][i].first.x;
      int height = new_tourist_locations[object_id - 1][i].second.y - new_tourist_locations[object_id - 1][i].first.y;
      Mat a = images_normalized_col[curimg](Rect(left, top, width, height));
      a.copyTo(img_object_col(Rect(left, top, width, height)));
    }
  }
  imwrite(output_folder_name + "touristless" + to_string(object_id) + ".jpg", img_object_col);
  cout << "touristlessization done" << endl;
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 5) {
    cout << "Wrong number of arguments. Given " << (argc - 1) << ", expected 4\n";
    cout << "Usage:" << endl;
    cout << "1) path to input folder with images" << endl;
    cout << "2) number of the best photo" << endl;
    cout << "3) path to tourist coordinates file (.txt)" << endl;
    cout << "4) path to output folder" << endl;
    return -1;
  }
  string input_folder_name = argv[1]; // "../testdata/occlusion_01/images/";
  int object_id = stoi(argv[2]);
  string toursit_locations_filename = argv[3]; // "../testdata/occlusion_01/coordinates.txt";
  string output_folder_name = argv[4]; // "../testdata/occlusion_01/images/output/");
  if (input_folder_name[input_folder_name.length() - 1] != '/') {
    input_folder_name.append("/");
  }
  if (output_folder_name[output_folder_name.length() - 1] != '/') {
    output_folder_name.append("/");
  }

  int n = get_number_of_files(input_folder_name.c_str());

  if (n < 2) {
    cout << "Not enough images for touristlessization. Given " << n << endl;
  }

  cout << "Starting to tourislessify" << endl;
  vector<vector<pair<Point2i, Point2i> > > tourist_locations = read_tourist_locations(toursit_locations_filename, n);
  if (tourist_locations.size() > 0) {
    touristlessization(input_folder_name, n, object_id, output_folder_name, tourist_locations);
  } else {
    cout << "Can not read tourist locations" << endl;
  }
  return 0;
}