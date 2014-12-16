project(io_tiff_jpeg)

add_executable(io_tiff_jpeg src/io_tiff_jpeg.cpp)

target_link_libraries(io_tiff_jpeg jpeg tiff)
