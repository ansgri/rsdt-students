project(char_another)
find_package(OpenCV REQUIRED)
add_executable(char_another
  src/task_fann.cpp
)

target_link_libraries(char_another
  ${OpenCV_LIBS}
)

target_link_libraries(char_another fann)