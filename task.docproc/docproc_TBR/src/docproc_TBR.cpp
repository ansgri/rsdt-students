#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>

using std::cout;
using std::endl;
using cv::Mat;
using cv::Size;

const int BLUR_APERTURE = 3;
const int MORPH_APERTURE = 41;
void show(const Mat& img, char title[])
{
	cv::imshow(title, img);
	cv::waitKey();
}

void save(const Mat& img, std::string name)
{
	cv::imwrite(name, img);
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		cout << "Bad usage: must have input image and output name as sole arg" << endl;
		return 1;
	}
	Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	std::string out_name = argv[2];
	Mat dst;

	cv::GaussianBlur(src, src, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
	//dst = src - dst;

	//show(src, "Blured");

	Mat const strel = cv::getStructuringElement(cv::MORPH_CROSS, Size(MORPH_APERTURE, MORPH_APERTURE));
	cv::morphologyEx(src, dst, cv::MORPH_CLOSE, strel);
	show(dst, "close_circle");
	dst = cv::Scalar(255) - (dst - src);
	show(dst, "morph");
	cv::threshold(dst, dst, 240, 255, 0);

	//show(dst, "Result");
	save(dst, out_name);
	return 0;
}