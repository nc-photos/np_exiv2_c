cmake_minimum_required(VERSION 3.18.1)
project(np_exiv2_c LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to Release as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
else()
  message(STATUS "Build type is '${CMAKE_BUILD_TYPE}'")
endif()

add_library(
  exiv2
  SHARED IMPORTED
)
set_target_properties(
  exiv2
  PROPERTIES IMPORTED_LOCATION
  ${EXIV2_LIBRARY}
)

find_library(log-lib log)
find_library(android-lib android)

add_library(
  np_exiv2_c
  SHARED
  src/np_exiv2_c.cpp
  src/reader.cpp
)
add_compile_options(
  -fvisibility=hidden
  -fvisibility-inlines-hidden
  -Wall
  -std=c++20
)
add_link_options(
  -fvisibility=hidden
  -fvisibility-inlines-hidden
)
target_include_directories(
  np_exiv2_c
  PRIVATE
  ${EXIV2_INCLUDE_DIR}
  ${EXIV2_BUILD_DIR}
)
target_link_libraries(
  np_exiv2_c
  ${log-lib}
  ${android-lib}
  exiv2
)
