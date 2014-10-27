project(io_png_tiff)
find_package(OpenCV REQUIRED)
add_executable(io_png_tiff
  src/main.cpp
)

target_link_libraries(io_png_tiff
  png
  tiff
  zlib
)
