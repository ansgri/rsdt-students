project(docproc_TBR)
find_package(OpenCV REQUIRED)
add_executable(docproc_TBR
  src/docproc_TBR.cpp
)

target_link_libraries(docproc_TBR
  ${OpenCV_LIBS}
)
