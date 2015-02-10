project(docproc_TCS)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(docproc_TCS src/docproc_TCS.cpp)

target_link_libraries(docproc_TCS ${OpenCV_LIBS})