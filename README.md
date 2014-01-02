cvblobslib
==========

Clone of http://sourceforge.net/p/cvblobslib as of 2013-12-29. The primary goal of this project is to create an updated and more readible branch of this library with defined coding standards. Further goals of this project are to convert the code to use the opencv2 API and finally to improve performance of the blobbing algorithm.

Building With CMake
=====================
Steps to build using cmake:
- mkdir build
- cd build
- cmake ..
  - To override the OpenCV location specify the following command line options
  - OpenCV_LIB_DIR - Directory where OpenCV libraries are located
  - OpenCV_INCLUDE_DIRS - Directory where OpenCV headers are located
  - Example: cmake -DOpenCV_LIB_DIR=/usr/local/lib -DOpenCV_INCLUDE_DIRS=/usr/local/include ..
  - These paths can be edited via ccmake using: ccmake ..
- make
- sudo make install

Backwards Compatibility
=======================
Many of the coding standards defined below break the API of the old versions of CvBlobs.
In order to maintain backwards compatibility the cvblobs/* headers will still be available.
The new API will be placed into cvblobs2/.

New Includes
============
For convenience i have created a few new header files in cvblobs/ and cvblobs2/
to ease use of the library.
- cvblobs/CvBlobs.h (and cvblobs2/CvBlobs.h) is a universal include header that includes
all header files in the library.
- cvblobs/CvBlobsDefs.h (and cvblobs2/CvBlobsDefs.h) header file which #defines all
macros for use in the library.
- cvblobs/CvBlobsFwd.h (and cvblobs2/CvBlobsFwd.h) is a header that forward declares
all classes, typedefs, and macros. This header only #includes CvBlobsDefs.h. 

Users should #include CvBlobsFwd.h in header files and CvBlobs.h in source files.


Coding Standards
=====================
- [ ] All classes begin with a capital letter (example: Blob)
- [ ] Everything will be defined within the cvblobs namespace
- [ ] Do not use exceptions
- [ ] Class constructors will perform complex work which could thrown an exception
- [ ] Function names start with a lower case and are camel cased names
- [ ] Accessor functions do not start with the word "get" instead just use the name of what is being accessed. Example: instead of GetBlob(i) it should be blob(i)
- [ ] Setter functions use the word "set". Example: setName(var);
- [ ] Doxygen comments will be made for ever class
- [ ] Doxygen comments will be made for all public and protected function
- [ ] All comments will be made in english
- [ ] Private variables will be prefixed with m. Example mSize
- [ ] Function arguments will be camel cased: Example numBlobs
- [ ] Local variables will be underscore delimited: Example my_local_var
- [ ] Pointers will be prefixed with a p: Example p_local_ptr, pBlobArg, mpPrivateBlob
- [ ] All file names will be camel case.
- [ ] All directories will be lower case with underscores separating words
- [x] Header guards will be formattted _CVBLOBS_FILENAME_H_ if in cvblobs/ directory
- [x] Header guards will be formattted _CVBLOBS2_FILENAME_H_ if in cvblobs2/ directory
- [ ] Prefer forward declarations to #include in headers
- [x] Do not use the "using" keyword
- [x] All includes will use <> and not quotes
- [ ] C++11 ... no for now
- [ ] Functions declared inline will be defined in the header file but not inside the class definition
- [ ] Best effort will be made for lines to not exceed 80 characters in width
- [ ] All braces will be on their own line
- [ ] All loops and conditionals will have braces
- [ ] All primitive variables will be initialized to 0 or 0.0 where declared and in all constructors if not initialized to another value.
- [ ] All pointers will be initialized to NULL where declared if not initialized to another value
- [ ] Only one variable will be declared per line. No compound declarations (double x, y, z;)
