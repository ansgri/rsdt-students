#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp" 

namespace fs = boost::filesystem;

#include <vector>


/**************** DISCUSSION ***************

  The solution definitely works and includes an added step
  for good picture selection, though the engineering part
  is far from perfect.

  1. Please use proper filesystem library to improve the UI
     (boost::filesystem is recommended though Qt is also OK):
    1.1. no need for the 'n' argument and fixed filename format,
         just list the directory contents using [0]
           [0] http://www.boost.org/doc/libs/1_39_0/libs/filesystem/doc/reference.html#Class-template-basic_directory_iterator
    1.2. fix the path join operation (dir + filename), current
         implementation gives bad results if out dir does not end
         with '/' (can also be fixed with boost::filesystem)
  2. you can select 'good' images by imshow()ing then and asking 'is it ok?'
     for each one with waitKey() and thus get rid of output dir;
     though the dir might be useful for debug purposes.
  3. method selection should probably be an option.
     look into argtable2 (included in this repo) or boost::program_options.
  3. See minor 'FIXME:' comments concerning style.

********************************************/

int main( int argc, char** argv )
{

  fs::path p_images(fs::current_path());
  fs::path f_model_image(fs::current_path());
  fs::path p_out(fs::current_path());

  if (argc == 4)
  {
    f_model_image = fs::system_complete(argv[1]);
    p_images = fs::system_complete(argv[2]);
    p_out = fs::system_complete(argv[3]);
  }
  else
  {
    std::cout << "\nusage: path_to_obj path_to_images output_file" << std::endl;
    return 1;
  }

  if (!fs::exists(p_images) || !fs::exists(f_model_image) || !fs::exists(p_images) )
  {
    std::cout << "\nCheck names. " << std::endl;
    return 1;
  }
  cv::Mat img_object = cv::imread(f_model_image.native(), CV_LOAD_IMAGE_GRAYSCALE );
  cv::SiftFeatureDetector detector(0, 4, 0.04, 10, 1.6);
  cv::SiftDescriptorExtractor extractor(3.0);

  std::vector<cv::KeyPoint> keypoints_object;
  detector.detect(img_object, keypoints_object); //detecting keypoints in obj

  cv::Mat descriptors_object;
  extractor.compute(img_object, keypoints_object, descriptors_object); 
                                                                       
  if (descriptors_object.empty())
  {
    cvError(0,"MatchFinder","1st descriptor empty",__FILE__,__LINE__);
  }

  std::vector<std::string> names;
  if (fs::is_directory(p_images))
  {
    fs::directory_iterator end_iter;
    for (fs::directory_iterator dir_itr(p_images);
          dir_itr != end_iter;
          ++dir_itr)
    {
      try
      {
        if (fs::is_regular_file(dir_itr->status()))
        {
          names.push_back(dir_itr->path().filename().native());
        }
      }
      catch (const std::exception & ex)
      {
        std::cout << dir_itr->path().filename() << " " << ex.what() << std::endl;
      }
    }
  }
  else
  {
    std::cout << "path_to_images must be dir.\n";
    return 1;
  }

  int n = names.size();
  std::vector<cv::Mat> images(n);

  for (int j = 0; j < n; j++)
  {

    std::vector<cv::KeyPoint> keypoints_scene;
    cv::Mat descriptors_scene;

    cv::Mat img_scene = cv::imread(p_images.native() + names[j], CV_LOAD_IMAGE_GRAYSCALE );
    cv::Mat img_scene_color = cv::imread(p_images.native()+names[j]);

    detector.detect(img_scene, keypoints_scene); 
    extractor.compute(img_scene, keypoints_scene, descriptors_scene);

    if (descriptors_scene.empty())
    {
      cvError(0,"MatchFinder","2nd descriptor empty",__FILE__,__LINE__);
    }

    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors_object, descriptors_scene, matches);

    double max_dist = 0; double min_dist = 100;

    for( int i = 0; i < descriptors_object.rows; i++ )
    {
      double dist = matches[i].distance;
      if( dist < min_dist ) min_dist = dist;
      if( dist > max_dist ) max_dist = dist;
    }

    std::vector< cv::DMatch > good_matches;
    for( int i = 0; i < descriptors_object.rows; i++ )
    {
      if( matches[i].distance < 3*min_dist )
      {
        good_matches.push_back( matches[i]);
      }
    }

    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;
    for( int i = 0; i < good_matches.size(); i++ )
    {
      obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
      scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }

    cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC );

    cv::warpPerspective(img_scene_color, img_scene_color, H.inv(), img_object.size());
    std::cout << "Writing image number " << j << " ...";
    img_scene_color.copyTo(images[j]);
    std::cout << "done\n";
  }
  std::cout << "Input 1 or 0 if image is good or bad and press ENTER\n";
  std::vector<int> good_results;
  bool is_good;

  for (int i = 0; i < n; i++)
  {
    cv::imshow("Image", images[i]);
    cv::waitKey(0);
    std::cin >> is_good;
    if(is_good)
      good_results.push_back(i);
  }

  if (good_results.size() == 0) 
    return 0;

  n = good_results.size();
 
  std::vector<int> pixel_R(n);
  std::vector<int> pixel_G(n);
  std::vector<int> pixel_B(n);

  int img_numb;

  cv::Mat flareless(images[0].rows, images[0].cols, CV_8UC3, cv::Scalar::all(0));

  for (int i = 0; i < images[0].rows; i++)
  {
    for (int j = 0; j < 3*images[0].cols; j+=3)
    {
      for (int k = 0; k < n; k++)
      {
        img_numb = good_results[k];
        uchar* scanline = images[img_numb].ptr<uchar>(i);

        pixel_R[k]= static_cast<uchar>(scanline[j]);
        pixel_G[k]= static_cast<uchar>(scanline[j+1]);
        pixel_B[k]= static_cast<uchar>(scanline[j+2]);
      }
        //MIN
      flareless.ptr<uchar>(i)[j] = static_cast<uchar>(*min_element(pixel_R.begin(), pixel_R.end()));
      flareless.ptr<uchar>(i)[j+1] = static_cast<uchar>(*min_element(pixel_G.begin(), pixel_G.end()));
      flareless.ptr<uchar>(i)[j+2] = static_cast<uchar>(*min_element(pixel_B.begin(), pixel_B.end()));
    }
  }
  cv::imwrite(p_out.native(), flareless);
}
