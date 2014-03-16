cvblobslib
==========
Clone of http://sourceforge.net/p/cvblobslib as of 2013-12-29. The primary goal of this project is to create an updated and more readible branch of this library with defined coding standards. Further goals of this project are to convert the code to use the opencv2 API and finally to improve performance of the blobbing algorithm.

Building With CMake
=====================
Steps to build using cmake:
- mkdir build
- cd build
- cmake ..
- make
- sudo make install

# to uninstall...
- sudo make uninstall

CMake Options
==============
- By default we try to find OpenCV using the cmake find_package(opencv) command.
  To override the OpenCV location specify the following command line options
  - OpenCV_LIB_DIR - Directory where OpenCV libraries are located
  - OpenCV_INCLUDE_DIRS - Directory where OpenCV headers are located
  - Example: cmake -DOpenCV_LIB_DIR=/usr/local/lib -DOpenCV_INCLUDE_DIRS=/usr/local/include ..
  - These paths can also be edited via ccmake using: ccmake ..
- BUILD_SHARED_LIBS - (default ON) Determines if cmake will build a shared or static library.
  To build a shared library set this to ON. To build a static library set this to OFF.
- CMAKE_BUILD_TYPE - (default Release) Determines the type of build to create.
  To build in debug mode set this to Debug. To build in release mode set this to Release.
- CMAKE_INSTALL_PREFIX - Determines where cmake will install the cvblobs files when you execute make install.

Generating Documentation
=======================
- mkdir build
- cd build
- cmake ..
- make doc

Documentation will be placed in build/doc/html


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
- cvblobs/CvBlobsDefs.h (and cvblobs2/CvBlobsDefs.h) header file that #defines all
macros for use in the library.
- cvblobs/CvBlobsFwd.h (and cvblobs2/CvBlobsFwd.h) is a header that forward declares
all classes, typedefs, and macros. This header only #includes CvBlobsDefs.h. 

Users should #include CvBlobsFwd.h in header files and CvBlobs.h in source files.


Coding Standards
=====================
- [x] All classes begin with a capital letter (example: Blob)
- [x] Everything will be defined within the cvblobs namespace
- [ ] Do not explicitly throw exceptions from our code. Ie never write the "throw" keyword. Exceptions may be thrown from STL functions as we use STL containers. I would like to guarauntee not throwing exceptions from our code, but this is very hard in C++.
- [ ] Document all exceptions that may be thrown from STL usage in a function.
- [x] Function names start with a lower case and are camel cased names
- [x] Accessor functions do not start with the word "get" instead just use the name of what is being accessed. Example: instead of GetBlob(i) it should be blob(i)
- [x] Setter functions use the word "set". Example: setName(var);
- [ ] Doxygen comments will be made for ever class
- [ ] Doxygen comments will be made for all public and protected function
- [ ] All comments will be made in english
- [x] Private variables will be prefixed with m. Example mSize
- [x] Function arguments will be camel cased: Example numBlobs
- [x] Local variables will be underscore delimited: Example my_local_var
- [x] Pointers will be prefixed with a p: Example p_local_ptr, pBlobArg, mpPrivateBlob
- [x] All file names will be camel case.
- [x] All directories will be lower case with underscores separating words
- [x] Header guards will be formattted _CVBLOBS_FILENAME_H_ if in cvblobs/ directory
- [x] Header guards will be formattted _CVBLOBS2_FILENAME_H_ if in cvblobs2/ directory
- [ ] Prefer forward declarations to #include in headers
- [x] Do not use the "using" keyword
- [x] All includes will use <> and not quotes
- [ ] C++11 ... no for now
- [x] Functions declared inline will be defined in the header file but not inside the class definition
- [ ] Best effort will be made for lines to not exceed 80 characters in width
- [x] All braces will be on their own line
- [x] All loops and conditionals will have braces
- [x] All primitive variables will be initialized to 0 or 0.0 where declared and in all constructors if not initialized to another value.
- [x] All pointers will be initialized to NULL where declared if not initialized to another value
- [x] Only one variable will be declared per line. No compound declarations (double x, y, z;)


TODOs
=========
- Put these TODOs in a better place
- Install documentation
- Migrate CMake to ProjectConfig style
- Split up the CMake files
- Testing framework