project(io_tiff_png)
add_executable(io_tiff_png
  src/prj.cpp
)

target_link_libraries(io_tiff_png
  tiff
  png
  zlib
)
