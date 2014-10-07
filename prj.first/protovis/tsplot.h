#pragma once
#include <opencv/cv.h>


namespace rsdt { namespace protovis {

class TimeSeriesPlot
{
public:
  TimeSeriesPlot
  (
    const cv::Scalar &color,
    const cv::Scalar &bgColor,
    const cv::Scalar &curColor,
    double            minValue,
    double            maxValue
  )
  : color(color),
    bgColor(bgColor),
    curColor(curColor),
    minValue(minValue),
    maxValue(maxValue),
    lastFrameNo(-1),
    lastY(-1),
    originFrameNo(-1)
  { }

  ~TimeSeriesPlot()
  { }

  void setCanvas(cv::Mat &canvas)
  {
    this->canvas = canvas;
  }

  void pushValue(int frameNo, double value);
private:
  cv::Mat    canvas;
  cv::Scalar color;
  cv::Scalar bgColor;
  cv::Scalar curColor;
  cv::Mat    lastPreCursorImage;
  int        lastFrameNo;
  int        lastY;
  int        originFrameNo;
  double     minValue;
  double     maxValue;
};

}} // rsdt::protovis
