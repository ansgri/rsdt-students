Documentation
=============

Intro
-----

This is a repo for "Recognition systems development tools" course. Students should have access to this repo.
Based on https://github.com/ansgri/cpp-boilerplate-minimal (rsdt branch at the time of writing).

Building sample
---------------

### Windows

Assuming Visual Studio 9 2008 as your sole installed compiler. Then from the root of the copied & customized cpp-boilerplate, run this:

 * mkdir build.vc9 && cd build.vc9
 * cmake ..
 * start <your-project-codename>.sln
 * Build Solution (F7)

To run the sample, OpenCV dlls must be in the system path.

### Linux

* mkdir build.cmake.debug && cd build.cmake.debug
* cmake .. -DCMAKE_BUILD_TYPE=Debug
* make -j5

Setting up Boost
----------------

If you already have Boost installed or built, just set the `BOOST_ROOT` environment variable, like this:

    set BOOST_ROOT=c:\Users\ansgri\thirdparty-src\boost_1_55_0

Otherwise, build it first following this approximate procedure:

 * you must have Visual Studio (or other compiler of choice) installed (obviously)
 * download and unpack the source archive
 * from the unpacked source dir execute `bootstrap.bat`
 * review the build options: `b2 --help`
 * build like this: `b2 link=shared variant=release threading=multi runtime-link=shared`

On Linux this is similar and generally more streamlined. Just RTFM.

Setting up OpenCV
-----------------

TODO: instructions for the case when OpenCV is installed via binary installer

Building from source on Windows:

 * `git clone git://github.com/Itseez/opencv.git`
 * `git checkout 2.4.6` (or other stable tag; to see all, execute `git tag`)
 * `mkdir build.user && cd build.user`
 * `cmake .. && start OpenCV.sln`
 * build solution (F7) in both Release and Debug configurations
 * build 'INSTALL' target in both Release and Debug configurations (in Solution Explorer, right-click and Build, it's in CMake Targets folder)
 * you'll have `install` folder under `build.user`. Its full path must be set to `OpenCV_DIR` environment variable.

