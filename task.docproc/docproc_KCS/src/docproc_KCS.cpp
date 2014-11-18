#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;

int main(int argc, char const** argv)
{
    Mat src,src2, dst, dst2;
    std::string image_name = argv[1];
    std::string out_image_name = argv[2];

    src = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);
    int BLUR_APERTURE = 100;
    Mat const strel = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));

    cv::morphologyEx(src, dst, cv::MORPH_ERODE, strel);
    cv::morphologyEx(src, dst2, cv::MORPH_DILATE, strel);

    dst.convertTo(dst, CV_32FC1);
    dst2.convertTo(dst2, CV_32FC1);
    src.convertTo(src, CV_32FC1);

    Mat ret = (src-dst)/(dst2-dst); // local auto contrast: brightet pxls -> white, darkest -> black
    ret = 255*ret;

    cv::imwrite(out_image_name, ret);

    return 0;
}
