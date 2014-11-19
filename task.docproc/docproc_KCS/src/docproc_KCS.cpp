#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;

int main(int argc, char const** argv)
{
    Mat src,src2, dst, dst2;
    std::string image_name = "/home/mastov/rsdt-students/bin.linux64.release/2.jpg";
    std::string out_image_name = "/home/mastov/rsdt-students/bin.linux64.release/out.jpg";
    std::string out_image_name1 = "/home/mastov/rsdt-students/bin.linux64.release/out1.jpg";

    src = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);
    //cv::resize(src, src, Size(),0.2, 0.2);
    int BLUR_APERTURE = 100;
    Mat const strel = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));

    cv::morphologyEx(src, dst, cv::MORPH_ERODE, strel);
    cv::morphologyEx(src, dst2, cv::MORPH_DILATE, strel);
    Mat ret2, ret3;
    // !!!!
    cv::threshold(dst2-dst,ret2, 50,255, 3); // applaying threshold to max-min difference. get drawing with "border"
    cv::threshold(dst2-dst,ret3, 50,255, 2); // white list
    // !!!!
    dst.convertTo(dst, CV_32FC1);
    dst2.convertTo(dst2, CV_32FC1);
    ret2.convertTo(ret2, CV_32FC1);
    ret3.convertTo(ret3, CV_32FC1);

    src.convertTo(src, CV_32FC1);

    Mat ret = (src-dst)/(dst2-dst); // old variant ,local auto contrast: brightet pxls -> white, darkest -> black
    Mat ret_new = (src-dst)/(ret2+0.001*ret3); // new. make list white, and does nothing with drawing
    ret = 255*ret;

    ret_new = 255*ret_new;

    //cv::imwrite(out_image_name, ret); //old
    cv::imwrite(out_image_name, ret_new); // new

    return 0;
}
