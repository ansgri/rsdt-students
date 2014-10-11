project(protovis)

find_package(OpenCV REQUIRED)

add_library(protovis
  canvas.h
  tsplot.h
  lineplot.h
  src/tsplot.cpp
  src/lineplot.cpp
)

target_link_libraries(protovis
  ${OpenCV_LIBS}
)
