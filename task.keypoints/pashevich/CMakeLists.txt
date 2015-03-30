project(touristless)
find_package(OpenCV REQUIRED)
add_executable(touristless
  src/touristless.cpp
)

target_link_libraries(touristless
  ${OpenCV_LIBS}
)
