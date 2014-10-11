#include <protovis/lineplot.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <protovis/lineplot.h>
#include <cmath>
#include <stdexcept>
#include <cstdio>


namespace rsdt { namespace protovis_sample {

static std::vector<float> get_sample_data(size_t size, float f2)
{
  if (!size)
    throw std::runtime_error("size == 0");

  std::vector<float> result(size, 0);
  for (size_t i = 0; i < size; ++i)
  {
    result[i] = cos(static_cast<float>(i) / size * 90) * sin(static_cast<float>(i) / size * f2) + 1;
  }

  return result;
}

void main(int argc, char **argv)
{
  if (argc < 2)
    throw std::runtime_error("wrong number of arguments");

  cv::Mat src = cv::imread(argv[1]);
  cv::Mat canvas_mat = src.clone();

  cv::Mat src_grey;
  cv::cvtColor(src, src_grey, CV_RGB2GRAY);

  int erosion_w = 0;
  while (true)
  {
    if (erosion_w >= 15)
      erosion_w = 0;

    cv::Mat eroded;
    cv::erode(src_grey, eroded, cv::Mat(), cv::Size(-1, -1), erosion_w);
    ++erosion_w;

    cv::Mat src_float;
    eroded.convertTo(src_float, CV_32FC1);

    protovis::Canvas canvas(canvas_mat);
    // canvas.set_scale(0.8, 5);
    // canvas.flip_y_axis();
    multiline_surface_plot(canvas, src_float, 12, 0, 0.1);

    // for (size_t i = 0; i < 60; ++i)
    // {
    //   canvas.shift_origin(0, -8);
    //   protovis::line_plot(canvas, get_sample_data(1000, 3 + 10 * i));
    // }

    cv::imshow("src", eroded);
    cv::imshow("canvas", canvas_mat);

    if ((cv::waitKey(300) & 0xFF) == 27)
      break;
  }
}

}}

int main(int argc, char **argv)
{
  try
  {
    rsdt::protovis_sample::main(argc, argv);
    return 0;
  }
  catch (std::exception const& e)
  {
    fprintf(stderr, "Exception: %s\n", e.what());
    return 1;
  }
}
