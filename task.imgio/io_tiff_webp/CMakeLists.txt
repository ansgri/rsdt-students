project(io_tiff_webp)
add_executable(io_tiff_webp
  src/io_tiff_webp.cpp
)

target_link_libraries(io_tiff_webp tiff libwebp zlib)