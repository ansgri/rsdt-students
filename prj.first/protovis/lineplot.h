#pragma once
#include <opencv/cv.h>


namespace rsdt { namespace protovis {

void verticalLinePlot
(
  cv::Mat &canvas,
  const cv::Scalar &color,
  int thickness,
  std::vector<float> const& data,
  float argScale,
  float valueScale
);

void plotHLine
(
  cv::Mat &canvas,
  int y,
  const cv::Scalar &color,
  int thickness
);

}} // rsdt::protovis
