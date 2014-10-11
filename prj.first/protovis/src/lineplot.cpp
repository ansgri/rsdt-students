#include "../lineplot.h"

namespace rsdt { namespace protovis {

void vertical_line_plot
(
  cv::Mat         & canvas,
  cv::Scalar const& color,
  int               thickness,
  std::vector<float> const& data,
  float             arg_scale,
  float             value_scale,
  cv::Point const&  origin
)
{
  cv::Point prev(origin);
  for (size_t i = 0; i < data.size(); ++i)
  {
    cv::Point next(static_cast<int>(data[i] * value_scale),
                   static_cast<int>(i * arg_scale));
    next += origin;
    cv::line(canvas, prev, next, color, thickness);
    prev = next;
  }
}

void plot_h_line
(
  cv::Mat & canvas,
  int       y,
  cv::Scalar const& color,
  int       thickness
)
{
  cv::line(canvas, cv::Point(0, y), cv::Point(canvas.cols, y), color, thickness);
}

}} // rsdt::protovis
