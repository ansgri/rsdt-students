#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>


using std::cout;
using std::endl;
using cv::Mat;
using cv::Size;

const int BLUR_APERTURE = 3;
const int MORPH_APERTURE = 41;

void show(const Mat& img, std::string title)
{
	cv::imshow(title, img);
	cv::waitKey();
}

void save(const Mat& img, std::string name)
{
	cv::imwrite(name, img);
}

Mat& rotate(cv::Mat& src, double angle, cv::Mat& dst)
{
    int len = std::max(src.cols, src.rows);
    cv::Point2f pt(len/2., len/2.);
    cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);

    cv::warpAffine(src, dst, r, cv::Size(len, len));
    return dst;
}

void binarization(char from[], char to[])
{
	Mat src = cv::imread(from, CV_LOAD_IMAGE_GRAYSCALE);
	std::string out_name = to;
	Mat dst;

	cv::GaussianBlur(src, src, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
	//dst = src - dst;

	//show(src, "Blured");

	Mat const strel = cv::getStructuringElement(cv::MORPH_CROSS, Size(MORPH_APERTURE, MORPH_APERTURE));
	cv::morphologyEx(src, dst, cv::MORPH_CLOSE, strel);
	//show(dst, "close_circle");
	dst = cv::Scalar(255) - (dst - src);

	//show(dst, "morph");
	cv::threshold(dst, dst, 238, 255, 0);

	//show(dst, "Result");
	save(dst, out_name);
}

double morph_mean(const Mat& src, double angle)
{
	Mat dst = src.clone();
	rotate(dst, angle, dst);
	Mat const strel = cv::getStructuringElement(cv::MORPH_RECT, Size(1, 8));
	cv::morphologyEx(dst, dst, cv::MORPH_OPEN, strel);
	// show(dst, "Morph");
	double mean = cv::mean(dst)[0];
	// cout << mean << endl;
	return mean;
}

void smart_rotation(char from[], char to[], int step = 1)
{
	Mat src = cv::imread(from, CV_LOAD_IMAGE_GRAYSCALE);
	std::string out_name = to;
	Mat dst = src.clone();

	cv::GaussianBlur(dst, dst, Size(BLUR_APERTURE, BLUR_APERTURE), 0);

	int scale = 1;
	int delta = 1;
	int ddepth = CV_16S;

	Mat grad_x, abs_grad_x;

	/// Gradient X
	Sobel(dst, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	// show(abs_grad_x, "grad");
	// Calculate max intensity from angle
	std::pair<double, double> max = std::make_pair(0, morph_mean(abs_grad_x, 0));
	for (double angle = step; angle < 360; angle += step)
	{
		double mean = morph_mean(abs_grad_x, angle);
		if (mean > max.second)
		{
			max = std::make_pair(angle, mean);
		}
	}
	save(rotate(src, max.first, src), to);
}

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		cout << "Bad usage: must have input image and 2 output name as sole arg" << endl;
		return 1;
	}
	binarization(argv[1], argv[2]);
	smart_rotation(argv[1], argv[3], 3);
	return 0;
}

