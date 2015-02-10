project(protovis)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
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
