#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;

int main(int argc, char const** argv)
{
	
    if(argc<2)
	{
		printf("%s\n","invalid argument, need path to image");
		return -1;
	}
	
	
    Mat src,local_minimum, local_maximum;
    std::string image_name = argv[1];
    std::string out_image_name = "local_autocontrast_out.jpg";
	//read image
    src = cv::imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);
    
    
	//create shape
    int BLUR_APERTURE = 100;
    Mat const shape = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));
    
    
	//apply morphology filters
    cv::morphologyEx(src, local_minimum, cv::MORPH_ERODE, shape);
    cv::morphologyEx(src, local_maximum, cv::MORPH_DILATE, shape);
    
    //convert to float
	local_minimum.convertTo(local_minimum, CV_32FC1);
	local_maximum.convertTo(local_maximum, CV_32FC1);
	src.convertTo(src, CV_32FC1);

	
	Mat contrast_result = 255*(src - local_minimum)/(local_maximum-local_minimum);
	cv::imwrite("local_autocontrast_out_2.jpg", contrast_result);
    return 0;
}
