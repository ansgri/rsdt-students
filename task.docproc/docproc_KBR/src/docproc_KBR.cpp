#include <stdio.h>
#include <opencv2/opencv.hpp>

static int const BLUR_APERTURE = 15;
static bool SAVE_SIDE_BY_SIDE = true;

using cv::Mat;
using cv::Size;

// docproc_KBR
// фото карандашного рисунка на белом листе (testdata/paper_lined.jpg)
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
//  cv::namedWindow("Display frame", CV_WINDOW_AUTOSIZE);
  Mat dst;
  cv::resize(src, dst, Size(src.cols / 3, src.rows / 3));
  cv::imshow("Display frame", dst);
  cv::waitKey();
}

//void save_filtered_images(Mat const& src)
//{
//  cv::medianBlur(src, dst, 91); // getting rid of drawing ( lines AND big black squares )
//  dst = cv::Scalar(255)- dst + src; // dst contains drawing
//  cv::threshold(dst,dst,245,255, 0); // binarising
//  
//  int BLUR_APERTURE = 2;
//  Mat const strel = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));
//  cv::morphologyEx(dst, dst, cv::MORPH_CLOSE, strel); // getting rid of "black dots" on paper
//  
//  cv::imwrite("filtered.jpg", dst);
//  cv::waitKey();
//}

void func2(Mat const& src)
{
  Mat dst;
  cv::blur(src, dst, Size(BLUR_APERTURE, BLUR_APERTURE));
//  save_result(dst);
  
  Mat mean;
  dst.convertTo(mean, CV_32FC1);
  mean = mean / 255;
  printf("convert ok\n");
//  mean = mean / 255;
  
//  mean = conv(dst) to float 1.0/255;
  Mat sqr = mean * mean;
  printf("sqr ok\n");
  
  Mat blur_sqr;
  cv::blur(sqr, blur_sqr, Size(BLUR_APERTURE, BLUR_APERTURE));
  printf("blur of sqr ok\n");
  
  Mat sd_f;
  sqrt(blur_sqr - mean * mean, sd_f);
  printf("sd_f ok\n");
//  save_result(sd_f);
  
  Mat threshold;
  float k = -0.1;
  threshold = mean + k * sd_f;
  save_result(threshold);
  
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
