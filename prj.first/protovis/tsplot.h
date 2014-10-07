#pragma once
#include <opencv/cv.h>


namespace rsdt { namespace protovis {

class TimeSeriesPlot
{
public:
  TimeSeriesPlot
  (
    const cv::Scalar &color,
    const cv::Scalar &bg_color,
    const cv::Scalar &cur_color,
    double            min_value,
    double            max_value
  )
  : color(color),
    bg_color(bg_color),
    cur_color(cur_color),
    min_value(min_value),
    max_value(max_value),
    last_frame_no(-1),
    last_y(-1),
    origin_frame_no(-1)
  { }

  ~TimeSeriesPlot()
  { }

  void set_canvas(cv::Mat &canvas)
  {
    this->canvas = canvas;
  }

  void push_value(int frameNo, double value);
private:
  cv::Mat    canvas;
  cv::Scalar color;
  cv::Scalar bg_color;
  cv::Scalar cur_color;
  cv::Mat    last_pre_cursor_image;
  int        last_frame_no;
  int        last_y;
  int        origin_frame_no;
  double     min_value;
  double     max_value;
};

}} // rsdt::protovis
