project(flareless)
find_package(OpenCV REQUIRED)
add_executable(flareless
  src/flareless.cpp
)

target_link_libraries(flareless
  ${OpenCV_LIBS}
)
