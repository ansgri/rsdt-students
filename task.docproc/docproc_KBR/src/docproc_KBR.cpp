#include <stdio.h>
#include <opencv2/opencv.hpp>

static int const BLUR_APERTURE = 3;
static int const MORPH_APERTURE = 40;
static int const MORPH_APERTURE_WIDTH = 3;
static int const MORPH_APERTURE_HEIGHT = 60;
static int const INITIAL_ANGLE = -360;

using cv::Mat;
using cv::Size;

// docproc_KBR
// бинаризовать
// развернуть

void save_result(cv::Mat const& src, std::string name)
{
  cv::imwrite(name, src);
  // Mat dst;
  // cv::resize(src, dst, Size(src.cols / 3, src.rows / 3));
  // cv::imshow(name, dst);
  // cv::waitKey();
  // cv::destroyWindow(name);
}

void rotate_mat(cv::Mat const& src, cv::Mat& dst, double angle)
{
    Mat rotation_maxtrix = cv::getRotationMatrix2D(cv::Point(src.cols / 2.0, src.rows / 2.0), angle, 1.0);
    cv::warpAffine(src, dst, rotation_maxtrix, src.size());
}

void rotation(cv::Mat const& src, std::string const image_path)
{
  Mat src_blur;
  cv::GaussianBlur(src, src_blur, Size(BLUR_APERTURE, BLUR_APERTURE), 0);

  Mat src_grad;
  Sobel(src_blur, src_grad, CV_16S, 1, 0);
  // save_result(src_grad, "grad");

  Mat src_grad_abs;
  convertScaleAbs(src_grad, src_grad_abs);
  // save_result(src_grad_abs, "abs_grad");

  double max_angle = INITIAL_ANGLE;
  double max = 0;
  double deviation = 30;
  double step = 1;

  for (double angle = 360 - deviation; angle <= 360 + deviation; angle += step)
  {
    Mat temp;
    rotate_mat(src_grad_abs, temp, (angle < 360 ? angle : angle - 360));
    Mat struct_element = cv::getStructuringElement(cv::MORPH_RECT, Size(MORPH_APERTURE_WIDTH, MORPH_APERTURE_HEIGHT));

    Mat temp_open;
    cv::morphologyEx(temp, temp_open, cv::MORPH_OPEN, struct_element);
    // cv::morphologyEx(temp, temp_close, cv::MORPH_CLOSE, struct_element);
    // save_result(temp_open, "morph_open");
    // save_result(temp_close, "morph_closed");
    double mean = cv::mean(temp_open)[0];



    // printf("mean = %f, angle = %f\n", mean, (angle < 360 ? angle : angle - 360));
    if (mean > max || max_angle == INITIAL_ANGLE)
    {
      max_angle = (angle < 360 ? angle : angle - 360);
      max = mean;
    }
  }
  // printf("max mean = %f, max angle = %f\n", max, max_angle);

  Mat result;
  rotate_mat(src, result, max_angle);
  // save_result(src, "src");
  save_result(result, image_path);
}

void binarization(Mat const& src, std::string const image_path)
{
  Mat src_blured;
  // cv::blur(src, src_blured, Size(BLUR_APERTURE, BLUR_APERTURE));
  cv::GaussianBlur(src, src_blured, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
  // save_result(src_blured, "blured");

  Mat src_ex;
  Mat struct_element = cv::getStructuringElement(cv::MORPH_CROSS, Size(MORPH_APERTURE, MORPH_APERTURE));
  cv::morphologyEx(src_blured, src_ex, cv::MORPH_CLOSE, struct_element);
  // save_result(src_ex, "morph");

  Mat reverse;
  reverse = cv::Scalar(255) - (src_ex - src_blured);
  // save_result(reverse, "reverse");

  Mat result;
  cv::threshold(reverse, result, 230, 255, 0);
  save_result(result, image_path);
}

int main(int argc, char const** argv)
{
  try
  {
    if (argc != 4)
      throw std::runtime_error("Bad usage: must have input image as sole arg");
    std::string const input_image_path = argv[1];
    std::string const bin_image_path = argv[2];
    std::string const rotate_image_path = argv[3];
    Mat const input_image = cv::imread(input_image_path, CV_LOAD_IMAGE_GRAYSCALE);
    
    binarization(input_image, bin_image_path);
    rotation(input_image, rotate_image_path);
    
    return 0;
  }
  catch (std::exception const& e)
  {
    fprintf(stderr, "Exception: %s\n", e.what());
    return 1;
  }
}
