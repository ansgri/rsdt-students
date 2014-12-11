project(io_jpeg_tiff)

add_executable(io_jpeg_tiff src/io_jpeg_tiff.cpp)

target_link_libraries(io_jpeg_tiff jpeg tiff)
