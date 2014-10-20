project(io_webp_png)
add_executable(io_webp_png
  src/main.cpp
)

target_link_libraries(io_webp_png
  png
  libwebp
  pthread
)
