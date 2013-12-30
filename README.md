cvblobslib
==========

Clone of http://sourceforge.net/p/cvblobslib as of 2013-12-29. The primary goal of this project is to create an updated and more readible branch of this library with defined coding standards. Further goals of this project are to convert the code to use the opencv2 API and finally to improve performance of the blobbing algorithm.

CMake
=====================
Steps to build using cmake:
1 - mkdir build
2 - cd build
3 - cmake ..
# to override the OpenCV location specify the following command line options
# OpenCV_LIB_DIR - Directory where OpenCV libraries are located
# OpenCV_INCLUDE_DIRS - Directory where OpenCV headers are located
# Example: cmake -DOpenCV_LIB_DIR=/usr/local/lib -DOpenCV_INCLUDE_DIRS=/usr/local/include ..
# These paths can be edited via ccmake also: ccmake ..
4 - make
5 - sudo make install