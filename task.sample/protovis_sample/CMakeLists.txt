project(protovis_sample)

find_package(OpenCV REQUIRED)
add_executable(protovis_sample
  src/protovis_sample.cpp
)
target_link_libraries(protovis_sample protovis)
