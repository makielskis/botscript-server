cmake_minimum_required(VERSION 2.6)
project(kyotocabinet)


################################
# Compile flags
################################
if(MSVC)
  add_definitions(/DNDEBUG)
  add_definitions(/D_CRT_SECURE_NO_WARNINGS)
  add_definitions(/EHsc)
  add_definitions(/W3)
  add_definitions(/wd4244)
  add_definitions(/wd4351)
  add_definitions(/wd4800)
else()
  add_definitions(-fPIC)
  add_definitions(-fsigned-char)
  add_definitions(-Wno-unused-but-set-variable)
  add_definitions(-Wno-unused-but-set-parameter)

  add_definitions(-DNDEBUG)
  add_definitions(-D_GNU_SOURCE=1)
  add_definitions(-D_FILE_OFFSET_BITS=64)
  add_definitions(-D_REENTRANT)
  add_definitions(-D__EXTENSIONS__)
  add_definitions(-D_MYZLIB)
  add_definitions(-D_MYGCCATOMIC)

  # Show all warnings.
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  set(CMAKE_CSS_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
endif()


################################
# Include Paths
################################
include_directories(".")


################################
# Object library
################################
add_library(kyotocabinet STATIC
  kcutil.cc
  kcthread.cc
  kcfile.cc
  kccompress.cc
  kccompare.cc
  kcmap.cc
  kcregex.cc
  kcdb.cc
  kcplantdb.cc
  kcprotodb.cc
  kcstashdb.cc
  kccachedb.cc
  kchashdb.cc
  kcdirdb.cc
  kctextdb.cc
  kcpolydb.cc
  kcdbext.cc
  kclangc.cc
)
