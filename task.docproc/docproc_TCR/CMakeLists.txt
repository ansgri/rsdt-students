project(docproc_TCR)
find_package(OpenCV REQUIRED)
add_executable(docproc_TCR
  src/docproc_TCR.cpp
)

target_link_libraries(docproc_TCR
  ${OpenCV_LIBS}
)
