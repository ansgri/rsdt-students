#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;

int main(int argc, char const** argv)
{
  if (!argv[1])
  {
    printf("You need to specify the input file\n");
    return 1;
  }
  if (!argv[2])
  {
    printf("You need to specify the output file\n");
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
  cv::imwrite(out_image_name, out);

  return 0;
}
