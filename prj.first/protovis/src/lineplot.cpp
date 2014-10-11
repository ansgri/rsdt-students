#include "../lineplot.h"

namespace rsdt { namespace protovis {

void line_plot
(
  Canvas          & canvas,
  std::vector<float> const& data
)
{
  if (data.empty())
    throw std::runtime_error("data empty");

  canvas.move_to(0, data[0]);
  for (int i = 1; i < static_cast<int>(data.size()); ++i)
  {
    canvas.line_to(i, data[i]);
  }
}

void multiline_surface_plot
(
  Canvas       & canvas,
  cv::Mat const& data,
  int            step,
  int            phase,
  float          z_scale
)
{
  canvas.flip_y_axis();
  canvas.set_origin(0, 0);
  for (int y = phase; y < data.rows; y += step)
  {
    canvas.shift_origin(0, step);
    std::vector<float> line(data.cols);
    float const*const row = data.ptr<float>(y);
    for (int x = 0; x < data.cols; ++x)
      line[x] = row[x] * z_scale;
    line_plot(canvas, line);
  }
}

}} // rsdt::protovis
