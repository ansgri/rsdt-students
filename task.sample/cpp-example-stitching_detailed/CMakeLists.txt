project(cpp-example-stitching_detailed)
find_package(OpenCV REQUIRED)

add_executable(cpp-example-stitching_detailed src/stitching_detailed.cpp)
target_link_libraries(cpp-example-stitching_detailed ${OpenCV_LIBS})
