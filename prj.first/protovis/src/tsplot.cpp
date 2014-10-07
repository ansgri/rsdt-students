#include "../tsplot.h"


namespace rsdt { namespace protovis {

void TimeSeriesPlot::push_value(int frame_no, double value)
{
  int y_baseline = canvas.rows;
  double scale = canvas.rows / (max_value - min_value);
  int y = y_baseline - static_cast<int>(scale * (value - min_value));
  if (last_y < 0)
    last_y = y;
  if (last_frame_no < 0)
    last_frame_no = frame_no;

  if (!last_pre_cursor_image.empty())
  {
    cv::Mat pre_cursor_image(canvas, cv::Rect(last_frame_no - origin_frame_no, 0, 1, canvas.rows));
    last_pre_cursor_image.copyTo(pre_cursor_image);
  }

  if (frame_no - origin_frame_no >= canvas.cols)
    origin_frame_no = last_frame_no;

  cv::rectangle(canvas,
                cv::Point(frame_no - origin_frame_no, 0),
                cv::Point(frame_no - origin_frame_no + 8, canvas.rows),
                bg_color,
                CV_FILLED);
  cv::line(canvas,
           cv::Point(last_frame_no - origin_frame_no, last_y),
           cv::Point(frame_no - origin_frame_no, y),
           color,
           1);

  cv::Mat pre_cursor_image(canvas, cv::Rect(frame_no - origin_frame_no, 0, 1, canvas.rows));
  last_pre_cursor_image = cv::Mat();
  pre_cursor_image.copyTo(last_pre_cursor_image);
  cv::line(canvas,
           cv::Point(frame_no - origin_frame_no, 0),
           cv::Point(frame_no - origin_frame_no, canvas.rows),
           cur_color);

  last_y = y;
  last_frame_no = frame_no;
}

}} // rsdt::protovis
