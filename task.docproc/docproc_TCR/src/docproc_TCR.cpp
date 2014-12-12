#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
static int const BLUR_APERTURE = 200;
static int const WIDTH = 5;
static int const HEIGHT = 100;

using cv::Mat;
using cv::Size;
double rotate( const Mat& src)
{
	Mat max; // picture with maximum average
	
	//long rectangle for morph filter
    Mat const shape = cv::getStructuringElement(0 , Size(WIDTH , HEIGHT));
    
    //parameters for rotation
	double angle = 0;
	double scale = 3;
	cv::Point center = cv::Point( src.cols/2 , src.rows/2 ); 
	
	bool flag = true; // to write first element in max
	
	for(int i =-45 ; i <= 45 ; i++)
	{
		//rotate pic and scale to remove white area arount the picture
		Mat warp_rotate_dst; // result picture
		Mat rot_mat = cv::getRotationMatrix2D( center , i , scale );
		cv::warpAffine( src, warp_rotate_dst , rot_mat , src.size() );
		
		Mat gradient_mat,mat, morph_mat;
		cv::Sobel( warp_rotate_dst , gradient_mat,CV_16S , 0 , 1 , 3 , 1 , 0 , cv::BORDER_DEFAULT ); //gradient
		cv::morphologyEx( gradient_mat , morph_mat , cv::MORPH_DILATE , shape );
		
		double average_max = cv::mean(max)[0];
		double average_current = cv::mean(morph_mat)[0];
		printf("%f %f \n", average_current, average_max);
		if(flag || average_current > average_max)
		{
			flag = false;
			angle = i;
			max = morph_mat;
			cv::imwrite("gradient_x.jpg",morph_mat);
		}
		
	}
	return angle;
	
	
} 



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
    Mat const shape = cv::getStructuringElement(0, Size(BLUR_APERTURE, BLUR_APERTURE));
    
    
	//apply morphology filters
    cv::morphologyEx(src, local_minimum, cv::MORPH_ERODE, shape);
    cv::morphologyEx(src, local_maximum, cv::MORPH_DILATE, shape);
    //convert to float
	local_minimum.convertTo(local_minimum, CV_32FC1);
	local_maximum.convertTo(local_maximum, CV_32FC1);
	src.convertTo(src, CV_32FC1);

	// Transform it into the C++ cv::Mat format
	Mat image(src); 

	// Setup a rectangle to define your region of interest
	cv::Rect myROI(src.cols/2-500, src.rows/2-500, 500, 500);

	// Crop the full image to that image contained by the rectangle myROI
	Mat cropped_image = image(myROI);
	double angle = rotate(cropped_image);
	
	Mat warp_rotate_dst;
	cv::Point center = cv::Point( src.cols/2, src.rows/2 ); 
	Mat rot_mat = cv::getRotationMatrix2D( center, angle, 1.1 );
	cv::warpAffine( 255*(src - local_minimum)/(local_maximum - local_minimum), warp_rotate_dst, rot_mat, src.size() );
	
	
	cv::imwrite("TCR_out.jpg", warp_rotate_dst);
    return 0;
}
