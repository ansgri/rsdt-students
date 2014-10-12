project(io_tiff_png)
find_package(OpenCV REQUIRED)
add_executable(io_tiff_png
  src/prj.cpp
)

target_link_libraries(io_tiff_png
  ${OpenCV_LIBS}
)
