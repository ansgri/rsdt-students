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

}} // rsdt::protovis
