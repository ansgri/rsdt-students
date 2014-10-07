#include "../tsplot.h"


namespace rsdt { namespace protovis {

void TimeSeriesPlot::pushValue(int frameNo, double value)
{
  int yBaseline = canvas.rows;
  double scale = canvas.rows / (maxValue - minValue);
  int y = yBaseline - static_cast<int>(scale * (value - minValue));
  if (lastY < 0)
    lastY = y;
  if (lastFrameNo < 0)
    lastFrameNo = frameNo;

  if (!lastPreCursorImage.empty())
  {
    cv::Mat preCursorImage(canvas, cv::Rect(lastFrameNo - originFrameNo, 0, 1, canvas.rows));
    lastPreCursorImage.copyTo(preCursorImage);
  }

  if (frameNo - originFrameNo >= canvas.cols)
    originFrameNo = lastFrameNo;

  cv::rectangle(canvas,
                cv::Point(frameNo - originFrameNo, 0),
                cv::Point(frameNo - originFrameNo + 8, canvas.rows),
                bgColor,
                CV_FILLED);
  cv::line(canvas,
           cv::Point(lastFrameNo - originFrameNo, lastY),
           cv::Point(frameNo - originFrameNo, y),
           color,
           1);

  cv::Mat preCursorImage(canvas, cv::Rect(frameNo - originFrameNo, 0, 1, canvas.rows));
  lastPreCursorImage = cv::Mat();
  preCursorImage.copyTo(lastPreCursorImage);
  cv::line(canvas,
           cv::Point(frameNo - originFrameNo, 0),
           cv::Point(frameNo - originFrameNo, canvas.rows),
           curColor);

  lastY = y;
  lastFrameNo = frameNo;
}

}} // rsdt::protovis
