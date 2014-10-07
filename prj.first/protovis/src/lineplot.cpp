#include "../lineplot.h"

namespace rsdt { namespace protovis {

void verticalLinePlot
(
  cv::Mat &canvas,
  const cv::Scalar &color,
  int thickness,
  std::vector<float> const& data,
  float argScale,
  float valueScale
)
{
  cv::Point prev(0, 0);
  for (size_t i = 0; i < data.size(); ++i)
  {
    cv::Point next(static_cast<int>(data[i] * valueScale),
                   static_cast<int>(i * argScale));
    cv::line(canvas, prev, next, color, thickness);
    prev = next;
  }
}

void plotHLine
(
  cv::Mat &canvas,
  int y,
  const cv::Scalar &color,
  int thickness
)
{
  cv::line(canvas, cv::Point(0, y), cv::Point(canvas.cols, y), color, thickness);
}

}} // rsdt::protovis
