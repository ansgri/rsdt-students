#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"

#include <vector>

using namespace std;
using namespace cv;

int main( int argc, char** argv )
{

  if (argc != 5)
  {
    cout << "Usage: path to object, path to folder with images, number of images, output folder  \n";
    return 1;
  }
  string path_to_obj = argv[1]; // "../testdata/06_ds50/obj.jpg"
  string scene_folder_name = argv[2]; //"../testdata/06_ds50/images/";
  int n = stoi(argv[3]); //5;
  string output_folder = argv[4]; //"../testdata/06_ds50/images/normolized/";

  Mat img_object = imread(path_to_obj, CV_LOAD_IMAGE_GRAYSCALE );
  //Mat img_scene_color = imread(scene_name);


  SiftFeatureDetector detector(0, 4, 0.04, 10, 1.6);
  SiftDescriptorExtractor extractor(3.0);

  std::vector<KeyPoint> keypoints_object;
  detector.detect(img_object, keypoints_object); //detecting keypoints in obj

  Mat descriptors_object;
  extractor.compute(img_object, keypoints_object, descriptors_object); //descripting them

  if (descriptors_object.empty())
  {
    cvError(0,"MatchFinder","1st descriptor empty",__FILE__,__LINE__);
  }

  for (int j = 1; j <= n; j++)
  {

    std::vector<KeyPoint> keypoints_scene;
    Mat descriptors_scene;

    Mat img_scene = imread(scene_folder_name+to_string(j)+".jpg", CV_LOAD_IMAGE_GRAYSCALE );
    Mat img_scene_color = imread(scene_folder_name+to_string(j)+".jpg");

    detector.detect(img_scene, keypoints_scene); //detecting keypoints in scene
    extractor.compute(img_scene, keypoints_scene, descriptors_scene);

    if (descriptors_scene.empty())
    {
     // cvError(0,"MatchFinder","2nd descriptor empty",__FILE__,__LINE__);
    }

    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors_object, descriptors_scene, matches);

    double max_dist = 0; double min_dist = 100;

    for( int i = 0; i < descriptors_object.rows; i++ )
    {
      double dist = matches[i].distance;
      if( dist < min_dist ) min_dist = dist;
      if( dist > max_dist ) max_dist = dist;
    }

    vector< DMatch > good_matches;
    for( int i = 0; i < descriptors_object.rows; i++ )
    {
      if( matches[i].distance < 3*min_dist )
      {
        good_matches.push_back( matches[i]);
      }
    }

    //Mat img_matches;
    vector<Point2f> obj;
    vector<Point2f> scene;
    for( int i = 0; i < good_matches.size(); i++ )
    {
      obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
      scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }

    Mat H = findHomography( obj, scene, CV_RANSAC );

    /*
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint(img_object.cols, 0);
    obj_corners[2] = cvPoint( img_object.cols, img_object.rows );
    obj_corners[3] = cvPoint(0, img_object.rows);

    std::vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);
    line( img_scene, scene_corners[0], scene_corners[1], Scalar(0, 255, 0), 4 );
    line( img_scene, scene_corners[1], scene_corners[2], Scalar( 0, 255, 0), 4 );
    line( img_scene, scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 4 );
    line( img_scene, scene_corners[3], scene_corners[0], Scalar( 0, 255, 0), 4 );
    */
    warpPerspective(img_scene_color, img_scene_color, H.inv(), img_object.size());
    cout << "Writing image number " << j << " ...";
    imwrite(output_folder+to_string(j)+".jpg", img_scene_color);
    cout << "done\n";
  }
  cout << "Look in a destination folder: " << output_folder << ".\nInput numbers of GOOD results below separated by ENTER ('0' in the end)\n";
  vector<int> good_results;
  int temp=-1;
  while (temp != 0)
  {
      cin >> temp;
      //cout << "   " << temp << "\n";
      good_results.push_back(temp);
  }

  if (good_results.size()==0) return 0;

  n = good_results.size()-1;
  vector<Mat> images(n);

  for (int i = 0; i < n; i++)
  {
    images[i] = imread(output_folder+to_string(good_results[i])+".jpg",1);
  }

  vector<int> pixel_R(n);
  vector<int> pixel_G(n);
  vector<int> pixel_B(n);

  Mat flareless(images[0].rows,images[0].cols, CV_8UC3, Scalar::all(0));

  for (int i = 0; i < images[0].rows; i++)
  {
    for (int j = 0; j < 3*images[0].cols; j+=3)
    {
      for (int img_numb = 0; img_numb < n; img_numb++)
      {

        uchar* scanline = images[img_numb].ptr<uchar>(i);

        pixel_R[img_numb]= (int)(scanline[j]);
        pixel_G[img_numb]= (int)(scanline[j+1]);
        pixel_B[img_numb]= (int)(scanline[j+2]);
      }

        /*
        // MEDIAN
        nth_element(pixel_R.begin(), pixel_R.begin()+n/2, pixel_R.end());
        nth_element(pixel_G.begin(), pixel_G.begin()+n/2, pixel_G.end());
        nth_element(pixel_B.begin(), pixel_B.begin()+n/2, pixel_B.end());
        flareless.ptr<uchar>(i)[j] = (uchar)(pixel_R[n/2]);
        flareless.ptr<uchar>(i)[j+1] = (uchar)(pixel_G[n/2]);
        flareless.ptr<uchar>(i)[j+2] = (uchar)(pixel_B[n/2]);
        */

        //MIN
        flareless.ptr<uchar>(i)[j] = (uchar)(*min_element(pixel_R.begin(), pixel_R.end()));
        flareless.ptr<uchar>(i)[j+1] = (uchar)(*min_element(pixel_G.begin(), pixel_G.end()));
        flareless.ptr<uchar>(i)[j+2] = (uchar)(*min_element(pixel_B.begin(), pixel_B.end()));

        /*
        //MAX
        flareless.ptr<uchar>(i)[j] = (uchar)(*max_element(pixel_R.begin(), pixel_R.end()));
        flareless.ptr<uchar>(i)[j+1] = (uchar)(*max_element(pixel_G.begin(), pixel_G.end()));
        flareless.ptr<uchar>(i)[j+2] = (uchar)(*max_element(pixel_B.begin(), pixel_B.end()));
        */
    }
  }
  imwrite(output_folder+"flareless.jpg", flareless);

  waitKey(0);
}
