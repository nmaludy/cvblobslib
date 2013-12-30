cmake_minimum_required(VERSION 2.6)

project(CvBlobsLib)

# name of the library file
set(CVBLOBS_LIBRARY_NAME "blob")

# verbose makefile
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Verbose" FORCE)
# debug build (-g) includes symbols
set(CMAKE_BUILD_TYPE Debug)
# warn all and -fPIC for shared build
set(CMAKE_CXX_FLAGS "-Wall -fPIC -fno-strict-aliasing")
# place the library in the lib/ directory by default
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib)

#######################
# Sources and Headers #
#######################

# CMAKE_CURRENT_LIST_DIR is the directory that contains the CMakeLists.txt file
# which in our case is the root directory of the project so add include/ to
# the include directories
include_directories(${CMAKE_CURRENT_LIST_DIR})

#### TODO ####
#### Move source and header files to src/ and include/ directories

# search the src/ directory for all files ending in .cpp
file(GLOB SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
# search the include/ directory for all files ending in .h
file(GLOB HEADERS ${CMAKE_CURRENT_LIST_DIR}/*.h)

# define the GeographicJni library and specify whether it is shared or not.
add_library(${CVBLOBS_LIBRARY_NAME} SHARED ${SOURCES} ${HEADERS})

##################################
# Find Packages and Dependencies #
##################################

# OpenCV
message("-- OpenCV - finding")
find_package(OpenCV REQUIRED)
if (DEFINED OpenCV_LIBS)
  list(REMOVE_DUPLICATES OpenCV_LIBS)
  message("-- OpenCV - found")
  message("-- OpenCV_VERSION - ${OpenCV_VERSION}")
  message("-- OpenCV_LIB_DIR - ${OpenCV_LIB_DIR}")
  message("-- OpenCV_LIBS - ${OpenCV_LIBS}")
  message("-- OpenCV_INCLUDE_DIRS - ${OpenCV_INCLUDE_DIRS}")

  include_directories(${OpenCV_INCLUDE_DIRS})
  link_directories(${OpenCV_LIB_DIR})
  target_link_libraries(${CVBLOBS_LIBRARY_NAME} opencv_core opencv_imgproc)
endif (DEFINED OpenCV_LIBS)

##################
# Debug Messages #
##################

# debug print to make sure we globbed the correct source/headers
foreach(header ${HEADERS})
  message("-- ${PROJECT_NAME} header: ${header}")
endforeach()

foreach(source ${SOURCES})
  message("-- ${PROJECT_NAME} source: ${source}")
endforeach()

# print out all of our include directories
get_property(dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message("-- ${PROJECT_NAME} include dir: ${dir}")
endforeach()

# print out of the libraries that will be linked to 
# these were added with target_link_libraries() above
list(REMOVE_DUPLICATES blob_LIB_DEPENDS)
foreach(lib ${blob_LIB_DEPENDS})
  message("-- ${PROJECT_NAME} link library: ${lib}")
endforeach()

##################
# Install Target #
##################
# usage: make install

# install headers
install (FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

# install library file
install(TARGETS ${CVBLOBS_LIBRARY_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib${LIB_SUFFIX}
  ARCHIVE DESTINATION lib${LIB_SUFFIX}
)

####################
# Uninstall target #
####################
# usage: make uninstall

# uninstalls everything that install installed
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)