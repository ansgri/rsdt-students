#include <cstdio>
#include <stdexcept>
#include <algorithm>
#include <opencv2/opencv.hpp>


using cv::Mat;
using cv::Size;


static int const BLUR_APERTURE = 15;
static double const BLUR_BILATERAL_SIGMA = 100;
static bool SAVE_SIDE_BY_SIDE = true;


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

static cv::Mat vstack(cv::Mat const& m1, cv::Mat const& m2)
{
  if (m1.empty())
    return m2.clone();
  if (m2.empty())
    return m1.clone();

  cv::Mat r(m1.rows + m2.rows, std::max(m1.cols, m2.cols), m1.type());
  r = cv::Scalar(0);
  cv::Mat d1 = r(cv::Rect(cv::Point(0, 0), m1.size()));
  cv::Mat d2 = r(cv::Rect(cv::Point(0, m1.rows), m2.size()));
  m1.copyTo(d1);
  m2.copyTo(d2);
  return r;
}


static void save_result(std::string const& name, cv::Mat const& src, cv::Mat const& dst)
{
  cv::imshow("filtered", hstack(src, dst));
  cv::waitKey(0);
  // cv::imwrite(name, SAVE_SIDE_BY_SIDE ? hstack(src, dst) : dst);
}


static void save_morph_images(Mat const& src, int strel_shape, std::string const& strel_name)
{
    int const BG_MORPH_APERTURE = 15;
    int const CONTRAST_MORPH_APERTURE = 100;
    
    Mat const strel_bg = cv::getStructuringElement(strel_shape, Size(BG_MORPH_APERTURE, BG_MORPH_APERTURE));
    Mat const strel_contrast = cv::getStructuringElement(strel_shape, Size(CONTRAST_MORPH_APERTURE, CONTRAST_MORPH_APERTURE));
    
    Mat background;
    
    cv::morphologyEx(src, background, cv::MORPH_CLOSE, strel_bg);
    save_result("background_" + strel_name + ".png", src, background);

    Mat foreground = cv::Scalar(255) - (background - src);
    save_result("foreground_" + strel_name + ".png", src, foreground);

    // autocontrast

    Mat fg_min;
    cv::erode(foreground, fg_min, strel_contrast);
    save_result("fg_min_" + strel_name + ".png", src, fg_min);

    Mat fg_max;
    cv::dilate(foreground, fg_max, strel_contrast);
    save_result("fg_max_" + strel_name + ".png", src, fg_max);

    Mat numerator_f;
    Mat(foreground - fg_min).convertTo(numerator_f, CV_32F);
    
    Mat denominator_f;
    Mat(fg_max - fg_min).convertTo(denominator_f, CV_32F);

    Mat fg_contrast_f = numerator_f / denominator_f;

    Mat fg_contrast;
    fg_contrast_f.convertTo(fg_contrast, CV_8U, 255, 0);

    save_result("fg_contrast_" + strel_name + ".png", src, fg_contrast);


    return;

    Mat dst;
    cv::dilate(src, dst, strel_bg);
    save_result("morph_dilate_" + strel_name + ".png", src, dst);

    cv::erode(src, dst, strel_bg);
    save_result("morph_erode_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_OPEN, strel_bg);
    save_result("morph_open_" + strel_name + ".png", src, dst);


    cv::morphologyEx(src, dst, cv::MORPH_GRADIENT, strel_bg);
    save_result("morph_grad_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_TOPHAT, strel_bg);
    save_result("morph_tophat_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_BLACKHAT, strel_bg);
    save_result("morph_blackhat_" + strel_name + ".png", src, dst);
}

static void save_filtered_images(Mat const& src)
{
    // save_result("src.png", src, src);

    { // such blocks '{}' are to ensure deallocation of filtered images to preserve memory
      // and also to enable reuse of the name 'dst'


        Mat src_f;
        src.convertTo(src_f, CV_32F, 1.0 / 255);
        Mat mean;
        cv::blur(src_f, mean, Size(BLUR_APERTURE, BLUR_APERTURE));
        
        Mat src_sqr_f = src_f * src_f;
        
        Mat mean_sqr_f;
        cv::blur(src_sqr_f, mean_sqr_f, Size(BLUR_APERTURE, BLUR_APERTURE));

        Mat sd_sqr_f = mean_sqr_f - mean * mean;
        Mat sd_f;
        cv::sqrt(sd_sqr_f, sd_f);
        
        Mat threshold_f = mean + (-0.1) * sd_f;

        Mat binarized = Mat::zeros(src.size(), CV_8UC1);
        Mat binarized_fg = Mat::ones(src.size(), CV_8UC1) * 255;
        
        binarized_fg.copyTo(binarized, src_f > threshold_f);
        save_result("binarized.png", src, binarized);

    }

    if (false)
    {
        Mat dst;
        cv::medianBlur(src, dst, BLUR_APERTURE);
        save_result("median.png", src, dst);
    }

    if (false)
    {
        Mat dst;
        cv::GaussianBlur(src, dst, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
        save_result("gauss.png", src, dst);
    }

    save_morph_images(src, cv::MORPH_RECT, "box");

    if (false)
    {
        Mat dst;
        cv::bilateralFilter(src, dst, BLUR_APERTURE, BLUR_BILATERAL_SIGMA, BLUR_BILATERAL_SIGMA);
        save_result("bilateral.png", src, dst);
    }

    if (false)
    {
        Mat smooth;
        cv::GaussianBlur(src, smooth, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
        
        Mat dst;
        cv::Canny(smooth, dst, 20, 80);
        save_result("canny.png", src, dst);
    }
}


int main(int argc, char const** argv)
{
    try
    {
        if (argc != 2)
            throw std::runtime_error("Bad usage: must have input image as sole arg");
        std::string const input_image_path = argv[1];
        Mat const input_image = cv::imread(input_image_path, CV_LOAD_IMAGE_GRAYSCALE);

        save_filtered_images(input_image);

        return 0;
    }
    catch (std::exception const& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}
