project(io_webp_png)
find_package(OpenCV REQUIRED)
add_executable(io_webp_png
  src/main.cpp
)

target_link_libraries(io_webp_png
  ${OpenCV_LIBS}
)
