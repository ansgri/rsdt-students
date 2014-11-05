#include <stdio.h>
#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;

int main(int argc, char const** argv)
{
    Mat src, dst;
    std::string image_name = argv[1];;

    src = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);

    cv::medianBlur(src, dst, 91); // getting rid of drawing ( lines AND big black squares )

    dst = cv::Scalar(255)- dst + src; // dst contains drawing
    cv::threshold(dst,dst,245,255, 0); // binarising

    int BLUR_APERTURE = 2;
    Mat const strel = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));
    cv::morphologyEx(dst, dst, cv::MORPH_CLOSE, strel); // getting rid of "black dots" on paper

    cv::imwrite("filtered.jpg", dst);
    cv::waitKey();
    return 0;
}
