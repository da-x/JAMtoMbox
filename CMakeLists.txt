cmake_minimum_required (VERSION 2.6)
project (JAMtoMbox)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

set(CMAKE_VERBOSE_MAKEFILE TRUE)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_executable(JAMtoMbox
 JAMtoMbox.cpp
 jamcrc32.c
 jamfetch.c
 jamfield.c
 jamhinfo.c
 jamlock.c
 jamlread.c
 jammbini.c
 jamscan.c
 jamstore.c
 jamsys.c
)
