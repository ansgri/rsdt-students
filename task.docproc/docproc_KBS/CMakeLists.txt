project(docproc_KBS)

find_package(OpenCV REQUIRED)

add_executable(docproc_KBS src/docproc_KBS.cpp)

target_link_libraries(docproc_KBS ${OpenCV_LIBS})