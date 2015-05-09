project(char_rec)
find_package(OpenCV REQUIRED)
add_executable(char_rec
  src/task3.cpp
)

target_link_libraries(char_rec
  ${OpenCV_LIBS}
)

target_link_libraries(char_rec fann)