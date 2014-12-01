#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;

int main(int argc, char const** argv)
{
    Mat src,src2, dst, dst2;
    std::string image_name = "2.jpg";
    std::string out_image_name = "out.jpg";
    std::string out_image_name1 = "out1.jpg";

    src = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);
    //cv::resize(src, src, Size(),0.2, 0.2);
    int BLUR_APERTURE = 300;
    int BLUR_APERTURE2 = 10;
    Mat const strel = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));
    Mat const strel2 = cv::getStructuringElement(0, Size(BLUR_APERTURE2, BLUR_APERTURE2));

    cv::morphologyEx(src, dst, cv::MORPH_TOPHAT, strel);
    cv::morphologyEx(dst, dst, cv::MORPH_BLACKHAT, strel2);
    cv::morphologyEx(src, dst2, cv::MORPH_TOPHAT, strel);
    cv::morphologyEx(dst2, dst2, cv::MORPH_BLACKHAT, strel2);
    cv::imwrite(out_image_name1,dst); // new
	cv::imwrite(out_image_name,  src - dst2); // new

    return 0;
}
