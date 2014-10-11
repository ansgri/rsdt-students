#pragma once
#include <opencv/cv.h>


namespace rsdt { namespace protovis {

void line_plot
(
  cv::Mat         & canvas,
  cv::Scalar const& color,
  int               thickness,
  std::vector<float> const& data,
  float             arg_scale,
  float             value_scale,
  cv::Point const&  origin
);

void plot_h_line
(
  cv::Mat & canvas,
  int       y,
  cv::Scalar const& color,
  int       thickness
);

}} // rsdt::protovis
