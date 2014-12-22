#include <stdio.h>
#include <opencv2/opencv.hpp>

static int const BLUR_APERTURE = 250;

using cv::Mat;
using cv::Size;

void binarization(char const* src_path, char const* dst_path)
{
  cv::Mat src = cv::imread(src_path, CV_LOAD_IMAGE_GRAYSCALE);
  if (!src.data)
  {
    throw std::invalid_argument("Error when read the file");
  }

  Mat src_f;
  src.convertTo(src_f, CV_32F, 1.0 / 255);
  Mat mean;
  cv::blur(src_f, mean, Size(BLUR_APERTURE, BLUR_APERTURE));

  Mat src_sqr_f = src_f.mul(src_f);

  Mat mean_sqr_f;
  cv::blur(src_sqr_f, mean_sqr_f, Size(BLUR_APERTURE, BLUR_APERTURE));

  Mat mean_sqr;
  cv::pow(mean, 2, mean_sqr);
  Mat sd_sqr_f = mean_sqr_f - mean_sqr;
  Mat sd_f;
  cv::sqrt(sd_sqr_f, sd_f);

  Mat threshold_f = mean + (-1.1) * sd_f;

  Mat binarized = Mat::zeros(src.size(), CV_8UC1);
  Mat binarized_fg = Mat::ones(src.size(), CV_8UC1) * 255;

  binarized_fg.copyTo(binarized, src_f > threshold_f);

  cv::imwrite(dst_path, binarized);
}

void skeletonization(char const* src_path, char const* dst_path)
{
  cv::Mat image = cv::imread(src_path, CV_LOAD_IMAGE_GRAYSCALE);
  if (!image.data)
  {
    throw std::invalid_argument("Error when read the file");
  }

  cv::Mat background;
  cv::medianBlur(image, background, 201);
  image = background - image;

  cv::threshold(image, image, 0.0, 255.0, cv::THRESH_OTSU);

  cv::Mat skelet(image.size(), CV_8UC1, cv::Scalar(0));
  cv::Mat tmp(image.size(), CV_8UC1);

  const cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

  while (true)
  {
    cv::morphologyEx(image, tmp, cv::MORPH_OPEN, element);
    skelet = skelet | (image - tmp);
    double max = 0;
    cv::erode(image, image, element);
    cv::minMaxLoc(image, 0, &max);
    if (max == 0)
      break;
  }

  cv::imwrite(dst_path, 255 - skelet);
}

int task_main(int argc, const char** argv)
{
  if (argc != 4)
  {
    std::cout << "Invalid arguments.\n";
    return 0;
  }
  try
  {
    binarization(argv[1], argv[2]);
    skeletonization(argv[1], argv[3]);
  }
  catch (std::exception e)
  {
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, const char** argv)
{
  int docproc_KBS = task_main(argc, argv);
  return 0;
}