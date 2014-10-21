project(io_jpeg_webp)
add_executable(io_jpeg_webp
  src/io_jpeg_webp.cpp
)

target_link_libraries(io_jpeg_webp jpeg libwebp
)
