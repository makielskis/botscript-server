cmake_minimum_required(VERSION 3.10)
project(botscript-server)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-D_WEBSOCKETPP_CPP11_STL_)
find_package(Threads)

if (MSVC)
  # PDB debug information is not supported by buildcache.
  string(REPLACE "/Zi" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
  string(REPLACE "/Zi" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  string(REPLACE "/Zi" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
  string(REPLACE "/Zi" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

################################
# Basic Compiler Flags
################################
foreach(flags CMAKE_C_FLAGS CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_DEBUG)
  string(REPLACE "-O3" "-O2" ${flags} "${${flags}}")
endforeach()

foreach(flags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG)
  string(REPLACE "-O3" "-O2" ${flags} "${${flags}}")
endforeach()


################################
# BotScript Server Version
################################
set(botscript_server_gui_address \"http://bot.makielski.net/ui/0.0.10\")

execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE botscript_server_git_branch
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND git --no-pager log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE botscript_server_git_ref
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND git describe --tags --dirty=-dirty --always
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE botscript_server_git_tag
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file (
  "src/botscript_server_version.h.in"
  "${PROJECT_SOURCE_DIR}/src/botscript_server_version.h"
)


################################
# Windows = Desktop Version
################################
if (MSVC OR MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDESKTOP")
endif()

set(SKIP_PERFORMANCE_COMPARISON On)
include(cmake/buildcache.cmake)
include(cmake/pkg.cmake)

################################
# Static library
################################
file(GLOB gui_files "src/win32gui/*.cc")
file(GLOB exe_files "src/main.cc")
file(GLOB jni_files "src/jni/*.cpp")
file(GLOB c_api_files "src/c_api/*.cc")
file(GLOB_RECURSE botscriptserver_lib_files "src/*.cc")
list(REMOVE_ITEM botscriptserver_lib_files ${gui_files})
list(REMOVE_ITEM botscriptserver_lib_files ${exe_files})
list(REMOVE_ITEM botscriptserver_lib_files ${jni_files})
list(REMOVE_ITEM botscriptserver_lib_files ${c_api_files})

add_library(botscriptserver_lib STATIC ${botscriptserver_lib_files})
target_include_directories(botscriptserver_lib PUBLIC src deps/websocketpp)
target_compile_features(botscriptserver_lib PUBLIC cxx_std_17)
target_link_libraries(botscriptserver_lib rapidjson bs dust-server cereal boost-program_options ${CMAKE_THREAD_LIBS_INIT})
if(MSVC)
  target_link_libraries(botscriptserver_lib bs dust boost)
  set(_def
    _VARIADIC_MAX=10
    _WIN32_WINNT=0x0601
  )
  set_target_properties(botscriptserver_lib PROPERTIES COMPILE_DEFINITIONS "${_def}")
else()
  if(MINGW)
    target_link_libraries(botscriptserver_lib
      boost
      -Wl,-Bstatic
        bs
        dust
        z
        ws2_32
        mswsock
      -static-libgcc
      -static-libstdc++
    )

    ### Compiler Definitions
    set(_def
      BOOST_THREAD_USE_LIB
      WINVER=0x0601
      _WIN32_WINNT=0x0601
      _WIN32_IE=0x0601
    )
    set_target_properties(botscriptserver_lib PROPERTIES COMPILE_DEFINITIONS "${_def}")

    ### Compiler Flags
    set_target_properties(botscriptserver_lib PROPERTIES COMPILE_FLAGS "-std=c++11 -s")
  else()
    set_target_properties(botscriptserver_lib PROPERTIES COMPILE_FLAGS "-Wall -Wextra -std=c++11")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      target_link_libraries(botscriptserver_lib bs dust boost z)
    elseif(ANDROID)
      target_link_libraries(botscriptserver_lib boost bs dust z)
    else()
      target_link_libraries(botscriptserver_lib boost bs dust z rt)
    endif()
  endif()
endif()


################################
# Static packages
################################
if (STATIC_PACKAGES)
  add_definitions(-DSTATIC_PACKAGES)
endif()


################################
# JNI shared library
################################
if (ANDROID)
  add_definitions(-DANDROID)
  add_library(botscript-server SHARED EXCLUDE_FROM_ALL ${jni_files})
  target_link_libraries(botscript-server c_bss)
  #include_directories(SYSTEM /System//Library/Frameworks/JavaVM.framework/Versions/A/Headers/)
  #include_directories(SYSTEM /usr/lib/jvm/java/include/)
  set(_def _WEBSOCKETPP_CPP11_STL_)
  set_target_properties(botscript-server PROPERTIES COMPILE_DEFINITIONS "${_def}")
  set_target_properties(botscript-server PROPERTIES COMPILE_FLAGS "-std=c++11")
  set_target_properties(botscript-server PROPERTIES SUFFIX ".so")

  add_custom_target(botscript-server-jar
    rm -rf android-lib &&
    mkdir -p android-lib/lib/armeabi &&
    ${ANDROID_STRIP_CMD} libbotscript-server.so &&
    cp libbotscript-server.so android-lib/lib/armeabi/ &&
    cd android-lib &&
    zip -9 -r ../armeabi.jar lib
  )
  add_dependencies(botscript-server-jar botscript-server)
endif()


################################
# C Library
################################
if (ANDROID)
  add_library(c_bss STATIC EXCLUDE_FROM_ALL ${c_api_files})
else()
  add_library(c_bss SHARED EXCLUDE_FROM_ALL ${c_api_files})
endif()
target_link_libraries(c_bss botscriptserver_lib)
set_target_properties(c_bss PROPERTIES COMPILE_FLAGS "-std=c++11")


################################
# GUI executable
################################
if (MINGW OR MSVC)
  add_executable(botscript-server-gui
    WIN32
    ${gui_files}
    src/win32gui/res/bot.rc
  )

  ### Library Dependencies
  target_link_libraries(botscript-server-gui
    -Wl,-Bstatic
      botscriptserver_lib
      ws2_32
      mswsock
    -static-libgcc
    -static-libstdc++
  )

  ### Compiler Definitions
  if (MINGW)
    set(_def
      BOOST_THREAD_USE_LIB
      _WIN32_WINNT=0x0601
      WINVER=0x0601
      _WIN32_IE=0x0601
    )
  else() # MSVC
    set(_def
      _VARIADIC_MAX=10
      _WIN32_WINNT=0x0601
    )
  endif()
  set_target_properties(botscript-server-gui PROPERTIES COMPILE_DEFINITIONS "${_def}")

  ### Compiler Flags
  if (MINGW)
    set_target_properties(botscript-server-gui PROPERTIES COMPILE_FLAGS "-std=c++11 -s")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
  endif()
endif()


################################
# Server executable
################################
add_executable(botscript-server-exe src/main.cc)
target_link_libraries(botscript-server-exe ${CMAKE_THREAD_LIBS_INIT})
if(MSVC)
  target_link_libraries(botscript-server-exe botscriptserver_lib dust-server)
  set(_def
    _VARIADIC_MAX=10
    _WIN32_WINNT=0x0601
  )
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_DEFINITIONS "${_def}")
elseif(MINGW)
  target_link_libraries(botscript-server-exe
    -Wl,-Bstatic
      botscriptserver_lib
      dust-server
      ws2_32
      mswsock
    -static-libgcc
    -static-libstdc++
  )

  ### Compiler Definitions
  set(_def
    BOOST_THREAD_USE_LIB
    WINVER=0x0601
    _WIN32_WINNT=0x0601
    _WIN32_IE=0x0601
  )
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_DEFINITIONS "${_def}")

  ### Compiler Flags
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_FLAGS "-std=c++11 -s")
else()
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_FLAGS "-Wall -Wextra -std=c++11")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(botscript-server-exe botscriptserver_lib dust-server)
  else()
    target_link_libraries(botscript-server-exe botscriptserver_lib dust-server rt)
  endif()
endif()


################################
# Unit Tests
################################
include_directories(SYSTEM ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(botscript-server-tests EXCLUDE_FROM_ALL
  test/allowed_users_parser_test.cpp
  test/error_test.cpp
  test/rapid_json_exception_test.cpp
  test/outgoing_messages_test.cpp
  test/db_bot_config_test.cpp
  test/user_test.cpp
  test/user_bot_config_test.cpp
  test/multi_index_test.cpp
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  target_link_libraries(botscript-server-tests gtest gtest_main botscriptserver_lib)
else()
  target_link_libraries(botscript-server-tests gtest gtest_main botscriptserver_lib)
endif()
set_target_properties(botscript-server-tests PROPERTIES COMPILE_FLAGS "-std=c++11")

add_test(botscript-server-tests botscript-server-tests)
