#include <opencv2/opencv.hpp>
using cv::Mat;
using cv::Size;

Mat rotate_img(Mat src, cv::Point center, float angle, float scale = 1.0)
{
  Mat ret;
  Mat rot_mat = getRotationMatrix2D(center, angle, scale);
  cv::warpAffine(src, ret, rot_mat, src.size());
  return ret;
}

float find_vert_direction(Mat& src)
{
  Mat const strel = cv::getStructuringElement(0, Size(1, 50));
  float max_angle = 80;
  int max_iter = 20;
  float min_mean = 500;
  float ret_angle = 0;

  for (int i = 0; i < max_iter; i++)
  {
    float angle = -max_angle + i*2*(max_angle/max_iter);
    Mat filtred;
    Mat rotated = rotate_img(src, cv::Point(src.cols/2, src.rows/2), angle); // rotate img
    cv::morphologyEx(rotated, filtred, cv::MORPH_DILATE, strel); // vertical lines are deleted if angle is wrong

    filtred = rotate_img(filtred, cv::Point(src.cols/2, src.rows/2), -angle); // rotate back
    float mean = cv::mean(filtred(cv::Rect(src.cols/4, src.rows/4, src.rows/4, src.rows/2)))[0];

    if (mean < min_mean)
    {
      min_mean = mean;
      ret_angle = angle;
    }

  }

  return ret_angle;
}

int main(int argc, char const** argv)
{
  if (argc != 3)
  {
    std::cout << "Bad usage: must have input image and ouput image as arg\n";
    return 1;
  }
  Mat src, erode_pic, dilate_pic;
  std::string image_name = argv[1] ;
  std::string out_image_name = argv[2];

  src = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);

  int BLUR_APERTURE = 100;
  Mat const strel = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));

  cv::morphologyEx(src, erode_pic, cv::MORPH_ERODE, strel);
  cv::morphologyEx(src, dilate_pic, cv::MORPH_DILATE, strel);
  cv::threshold(erode_pic,erode_pic, 50, 50, 2);

  erode_pic.convertTo(erode_pic, CV_32FC1);
  dilate_pic.convertTo(dilate_pic, CV_32FC1);
  src.convertTo(src, CV_32FC1);

  Mat out = (src - erode_pic)/(dilate_pic - erode_pic);
  out = 255*out;

  float angle = find_vert_direction(out);

  cv::imwrite(out_image_name, rotate_img(out, cv::Point(src.cols/2, src.rows/2), angle));

  return 0;
}
