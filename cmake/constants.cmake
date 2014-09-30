# --- architecture ---
if(WIN32)
  set(CPPBP_OPERATING_SYSTEM "win")
elseif(UNIX)
  set(CPPBP_OPERATING_SYSTEM "linux")
else()
  message(FATAL_ERROR "Unknown operating system")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL "4")
  set(CPPBP_CPU_BITNESS "32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL "8")
  set(CPPBP_CPU_BITNESS "64")
else()
  message(FATAL_ERROR "Unsupported CPU bitness")
endif()

set(CPPBP_ARCH ${CPPBP_OPERATING_SYSTEM}${CPPBP_CPU_BITNESS})
message(STATUS "Target architecture: ${CPPBP_ARCH}")

# --- directories ---
set(CPPBP_ROOT ${rsdt_SOURCE_DIR})
set(CPPBP_INCLUDE_ROOT ${CPPBP_ROOT}/include)
set(CPPBP_LIB_ROOT ${CPPBP_ROOT}/lib/${CPPBP_ARCH})
if(WIN32 AND NOT MSVC_IDE)
  set(CPPBP_LIB_ROOT ${CPPBP_LIB_ROOT} ${CPPBP_LIB_ROOT}/${CMAKE_BUILD_TYPE})
endif()



# --- lists of libraries for linking ---
if(WIN32)
  set(CPPBP_OPENCV_LIBS
    optimized opencv_core249
    optimized opencv_highgui249
    optimized opencv_imgproc249
    optimized opencv_video249
    debug opencv_core249d
    debug opencv_highgui249d
    debug opencv_imgproc249d
    debug opencv_video249d
  )
elseif(UNIX)
  set(CPPBP_OPENCV_LIBS
    opencv_core
    opencv_highgui
    opencv_imgproc
    opencv_video
  )
endif()
