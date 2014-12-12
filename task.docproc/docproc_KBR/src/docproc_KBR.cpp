#include <stdio.h>
#include <opencv2/opencv.hpp>

static int const BLUR_APERTURE = 200;

using cv::Mat;
using cv::Size;

// docproc_KBR
// бинаризовать
// развернуть

static cv::Mat hstack(cv::Mat const& m1, cv::Mat const& m2)
{
  if (m1.empty())
    return m2.clone();
  if (m2.empty())
    return m1.clone();
  
  cv::Mat r(std::max(m1.rows, m2.rows), m1.cols + m2.cols, m1.type());
  r = cv::Scalar(0);
  cv::Mat d1 = r(cv::Rect(cv::Point(0, 0), m1.size()));
  cv::Mat d2 = r(cv::Rect(cv::Point(m1.cols, 0), m2.size()));
  m1.copyTo(d1);
  m2.copyTo(d2);
  return r;
}

static void save_result(cv::Mat const& src)
{
  Mat dst;
  cv::resize(src, dst, Size(src.cols / 4, src.rows / 4));
  cv::imshow("Display frame", dst);
  cv::waitKey();
}

void func2(Mat const& src)
{
  Mat src_float;
  src.convertTo(src_float, CV_32FC1);

  Mat src_float_n;
  src_float_n = src_float * (1.0 / 255);

  Mat mean_float_n;
  cv::blur(src_float_n, mean_float_n, Size(BLUR_APERTURE, BLUR_APERTURE));

  Mat sqr_src_float_n = src_float_n.mul(src_float_n);
  printf("sqr_src_float_n ok\n");
  
  Mat mean_sqr_src_float_n;
  cv::blur(sqr_src_float_n, mean_sqr_src_float_n, Size(BLUR_APERTURE, BLUR_APERTURE));
  printf("mean_sqr_src_float_n ok\n");
  
  Mat var_float_n;
  var_float_n = mean_sqr_src_float_n - mean_float_n.mul(mean_float_n);
  printf("var_float_n ok\n");

  Mat result;
  sqrt(var_float_n, result);
  printf("sd_f ok\n");
  // save_result(sd_f);

  Mat threshold;
  float k = -0.9;
  float d = 0.00;
  threshold = mean_float_n + k * result + d;
  // save_result(threshold);

  Mat binarized;
  binarized = Mat::zeros(src.size(), CV_32FC1);

  src_float_n.copyTo(binarized, src_float_n > threshold);
  save_result(binarized);
  
//  Mat binarized;
//  binarized = Mat::zeros(src.size, CV_8UC1);
  
//  Mat binarized_fg;
//  binarized_fg = Mat::ones(src.size, CV_8UC1);
  
//  bin_fg.copyTo(binarized, src_f > thershold);
  
  // binarized[src_f > thershold] = cv::Scalar(255)
//  save_result(binarized);
}

int main(int argc, char const** argv)
{
  try
  {
    if (argc != 2)
      throw std::runtime_error("Bad usage: must have input image as sole arg");
    std::string const input_image_path = argv[1];
    Mat const input_image = cv::imread(input_image_path, CV_LOAD_IMAGE_GRAYSCALE);
    
    func2(input_image);
    
    return 0;
  }
  catch (std::exception const& e)
  {
    fprintf(stderr, "Exception: %s\n", e.what());
    return 1;
  }
}
