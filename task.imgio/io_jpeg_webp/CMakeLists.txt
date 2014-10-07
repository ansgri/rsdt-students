project(io_jpeg_webp)
find_package(OpenCV REQUIRED)
add_executable(io_jpeg_webp
  src/camshiftdemo.cpp
)

target_link_libraries(io_jpeg_webp
  ${OpenCV_LIBS}
)
