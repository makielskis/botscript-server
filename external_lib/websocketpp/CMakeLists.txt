cmake_minimum_required(VERSION 2.8.8)
project(websocketpp)

################################
# Boost
################################
if(MSVC)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/boost_1_52_0")
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/boost_1_52_0/stage/lib")
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
	message("No Boost found.")
endif(Boost_FOUND)
link_directories(${CMAKE_LIBRARY_PATH})

if(MSVC)
else()
	# Enable building a shared library.
	add_definitions("-fPIC")
endif()

include_directories("include")
set(SOURCES
  src/network_utilities.cpp
  src/sha1/sha1.cpp
  src/base64/base64.cpp
  src/md5/md5.c
  src/uri.cpp
  src/processors/hybi_header.cpp
  src/processors/hybi_util.cpp
  src/messages/data.cpp
)

add_library(websocketpp STATIC ${SOURCES})
