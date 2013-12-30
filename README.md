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
- [ ] Header guards will be formattted _CVBLOBSLIB_FILENAME_H_
- [ ] Prefer forward declarations to #include in headers
- [x] Do not use the "using" keyword
- [x] All includes will use <> and not quotes
- [ ] C++11 ... no for now

