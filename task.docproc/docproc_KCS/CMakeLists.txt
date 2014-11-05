project(docproc_KCS)
find_package(OpenCV REQUIRED)
add_executable(docproc_KCS
  src/docproc_KCS.cpp
)

target_link_libraries(docproc_KCS
  ${OpenCV_LIBS}
)
