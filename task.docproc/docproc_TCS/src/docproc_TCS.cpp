#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdexcept>

void autocontrast(char const* src_path, char const* dst_path)
{
  cv::Mat image = cv::imread(src_path, CV_LOAD_IMAGE_GRAYSCALE);
  if (!image.data)
    throw std::invalid_argument("Error when read the file");

  cv::Mat er_img, dil_img;
  int erosion_size = 3;  

  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
    cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
    cv::Point(erosion_size, erosion_size));

  cv::erode(image, er_img, element);
  cv::dilate(image, dil_img, element);

  er_img.convertTo(er_img, CV_32FC1);
  dil_img.convertTo(dil_img, CV_32FC1);
  image.convertTo(image, CV_32FC1);

  double min = 0;
  cv::minMaxLoc(dil_img, &min);

  cv::Mat er_img_thrld(er_img.rows, er_img.cols, CV_32FC1, 0.0);  
  cv::min(er_img, min, er_img_thrld);

  cv::Mat res = (image - er_img_thrld) / (dil_img - er_img_thrld); 
  res *= 255;
  cv::imwrite(dst_path, res);
}

int main(int argc, const char** argv)
{
  if (argc != 3)
    std::cout << "Invalid arguments.\n";
  try
  {
    autocontrast(argv[1], argv[2]);
  }
  catch(std::exception e)
  { std::cout << e.what() << std::endl; }

  return 0;
}