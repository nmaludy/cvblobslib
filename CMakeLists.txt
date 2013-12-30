cmake_minimum_required(VERSION 2.6)

project(CvBlobsLib)

# name of the library file
set(CVBLOBS_LIBRARY_NAME "blob")

# verbose makefile
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Verbose" FORCE)
# debug build (-g) includes symbols
set(CMAKE_BUILD_TYPE Debug)
# warn all and -fPIC for shared build
set(CMAKE_CXX_FLAGS "-Wall -fPIC -fno-strict-aliasing -ansi -pipe -fstrength-reduce -finline-functions -Wall -Wno-unused-result")
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
# Option 1 - don't define anything and let is find the package
# Option 2 - specify OpenCV_LIB_DIR and OpenCV_INCLUDE_DIRS on the command line
# TODO document this
if (DEFINED OpenCV_LIB_DIR AND
    NOT DEFINED OpenCV_INCLUDE_DIRS)
  message(FATAL_ERROR "-- OpenCV - OpenCV_LIB_DIR defined on command line but you forgot to define OpenCV_INCLUDE_DIRS")
endif()

if (DEFINED OpenCV_INCLUDE_DIRS AND
    NOT DEFINED OpenCV_LIB_DIR)
  message(FATAL_ERROR "-- OpenCV - OpenCV_INCLUDE_DIRS defined on command line but you forgot to define OpenCV_LIB_DIRS")
endif()

if (DEFINED OpenCV_LIB_DIR AND
    DEFINED OpenCV_INCLUDE_DIRS)
  # user specified variables on command line (or they are in the cache already)
  message("-- OpenCV - defined on command line or in ccmake cache")
  message("-- OpenCV_LIB_DIR - ${OpenCV_LIB_DIR}")
  message("-- OpenCV_INCLUDE_DIRS - ${OpenCV_INCLUDE_DIRS}")
  
  include_directories(${OpenCV_INCLUDE_DIRS})
  link_directories(${OpenCV_LIB_DIR})
  target_link_libraries(${CVBLOBS_LIBRARY_NAME} opencv_core opencv_imgproc)
else ()
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
endif (DEFINED OpenCV_LIB_DIR AND 
       DEFINED OpenCV_INCLUDE_DIRS)

# cache variables
# use FORCE to update the cache with the latest value
set(OpenCV_LIB_DIR ${OpenCV_LIB_DIR} CACHE PATH "Directory where OpenCV libraries are located" FORCE)
set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} CACHE PATH "Directory where OpenCV headers are located" FORCE)

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