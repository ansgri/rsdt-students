#include <protovis/lineplot.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
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
  cv::Mat canvas_mat = cv::Mat::zeros(600, 800, CV_8UC3);
  protovis::Canvas canvas(canvas_mat);
  canvas.set_scale(0.8, 5);

  for (size_t i = 0; i < 60; ++i)
  {
    canvas.set_origin(0, canvas_mat.rows - 20 - i * 8);
    protovis::line_plot(canvas, get_sample_data(1000, 3 + 10 * i));
  }

  cv::imshow("src", src);
  cv::imshow("canvas", canvas_mat);
  while ((cv::waitKey(100) & 0xFF) != 27)
  {
    // nothing
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
