cmake_minimum_required(VERSION 2.6)
project(botscript-server)


################################
# Compile flags
################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
if(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
  add_definitions(
    -DBOOST_THREAD_USE_LIB
    -D_WIN32_WINNT=0x0501
    -DWINVER=0x0501
    -D_WIN32_IE=0x0501
  )
elseif(ANDROID)
  add_definitions(-DANDROID)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DOS_MACOSX)
endif()


################################
# Dependencies
################################
add_subdirectory(external_lib/botscript)
add_subdirectory(external_lib/dust)


################################
# Boost
################################
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS
  system
  thread
  date_time
  filesystem
  iostreams
  regex
  random
REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
	message("No Boost found.")
endif(Boost_FOUND)
link_directories(${CMAKE_LIBRARY_PATH})


################################
# Include Paths
################################
include_directories("external_lib/websocketpp")
include_directories("external_lib/botscript/src")
include_directories("external_lib/botscript/external_lib/rapidjson/include")
include_directories("external_lib/botscript/external_lib/pugixml-1.2/src")
include_directories("external_lib/botscript/external_lib/lua-5.2.0/src")
include_directories("external_lib/dust/include")


################################
# Static library
################################
file(GLOB gui_files "src/win32gui/*.cc")
file(GLOB exe_files "src/main.cc")
file(GLOB jni_files "src/jni/*.cpp")
file(GLOB_RECURSE botscriptserver_lib_files "src/*.cc")
list(REMOVE_ITEM botscriptserver_lib_files ${gui_files})
list(REMOVE_ITEM botscriptserver_lib_files ${exe_files})
list(REMOVE_ITEM botscriptserver_lib_files ${jni_files})
add_library(botscriptserver_lib STATIC ${botscriptserver_lib_files})
target_link_libraries(botscriptserver_lib bs dust z)


################################
# JNI shared library
################################
add_library(botscript-server SHARED EXCLUDE_FROM_ALL ${jni_files})
#include_directories(/usr/lib/jvm/java/include/)
set_target_properties(botscript-server PROPERTIES COMPILE_FLAGS "-DDESKTOP")


if(MINGW)
################################
# GUI executable
################################
  add_executable(botscript-server-gui
    WIN32
    ${gui_files}
    src/win32gui/res/bot.rc
  )
  set_target_properties(botscript-server-gui PROPERTIES COMPILE_FLAGS "-DDESKTOP")
  target_link_libraries(botscript-server-gui -Wl,-Bstatic botscriptserver_lib ws2_32 mswsock -static-libgcc -static-libstdc++)
else()
################################
# Server executable
################################
  add_executable(botscript-server-exe src/main.cc)

  if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(botscript-server-exe botscriptserver_lib)
  else()
    target_link_libraries(botscript-server-exe botscriptserver_lib rt -pthread)
  endif()
endif()


################################
# Unit Tests
################################
if (NOT TARGET gtest)
  add_subdirectory(test/gtest-1.7.0)
  enable_testing()
endif()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(botscript-server-tests EXCLUDE_FROM_ALL
  test/error_test.cpp
  test/rapid_json_exception_test.cpp
  test/outgoing_messages_test.cpp
  test/db_bot_config_test.cpp
  test/user_test.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  target_link_libraries(botscript-server-tests gtest gtest_main botscriptserver_lib)
else()
  target_link_libraries(botscript-server-tests gtest gtest_main botscriptserver_lib -pthread)
endif()
