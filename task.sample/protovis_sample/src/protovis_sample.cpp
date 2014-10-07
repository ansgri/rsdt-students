#include <protovis/lineplot.h>
#include <opencv2/highgui/highgui.hpp>
#include <stdexcept>
#include <cstdio>


namespace rsdt { namespace protovis_sample {

void main(int argc, char **argv)
{
  if (argc < 2)
    throw std::runtime_error("wrong number of arguments");

  auto const src = cv::imread(argv[1]);
  cv::imshow("src", src);
  while ((cv::waitKey(100) & 0xFF) != 27)
  {
    // nothing
  }
}

}}

int main(int argc, char **argv)
{
  try
  {
    rsdt::protovis_sample::main(argc, argv);
    return 0;
  }
  catch (std::exception const& e)
  {
    fprintf(stderr, "Exception: %s\n", e.what());
    return 1;
  }
}
