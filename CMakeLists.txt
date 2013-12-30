set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)

cmake_minimum_required(VERSION 2.6)
project(botscript-server)


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
if(MSVC)
  target_link_libraries(botscriptserver_lib bs dust)
  set(_def
    _VARIADIC_MAX=10
    _WIN32_WINNT=0x0501
  )
  set_target_properties(botscriptserver_lib PROPERTIES COMPILE_DEFINITIONS "${_def}")
else()
  target_link_libraries(botscriptserver_lib bs dust z)
  if(MINGW)
    target_link_libraries(botscriptserver_lib
      -Wl,-Bstatic
        ws2_32
        mswsock
      -static-libgcc
      -static-libstdc++
    )

    ### Compiler Definitions
    set(_def
      BOOST_THREAD_USE_LIB
      WINVER=0x0501
      _WIN32_WINNT=0x0501
      _WIN32_IE=0x0501
    )
    set_target_properties(botscriptserver_lib PROPERTIES COMPILE_DEFINITIONS "${_def}")

    ### Compiler Flags
    set_target_properties(botscriptserver_lib PROPERTIES COMPILE_FLAGS "-std=c++11 -s")
  else()
    set_target_properties(botscriptserver_lib PROPERTIES COMPILE_FLAGS "-Wall -Wextra -std=c++11")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      target_link_libraries(botscriptserver_lib bs dust z)
    else()
      target_link_libraries(botscriptserver_lib bs dust z rt -pthread)
    endif()
  endif()
endif()


################################
# JNI shared library
################################
if (ANDROID)
  add_definitions(-DANDROID)
  add_library(botscript-server SHARED EXCLUDE_FROM_ALL ${jni_files})
  #include_directories(/usr/lib/jvm/java/include/)
  set_target_properties(botscript-server PROPERTIES COMPILE_FLAGS "-DDESKTOP")
endif()


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
      _WIN32_WINNT=0x0501
      WINVER=0x0501
      _WIN32_IE=0x0501
      DESKTOP
    )
  else() # MSVC
    set(_def
      _VARIADIC_MAX=10
      _WIN32_WINNT=0x0501
      DESKTOP
    )
  endif()
  set_target_properties(botscript-server-gui PROPERTIES COMPILE_DEFINITIONS "${_def}")

  ### Compiler Flags
  if (MINGW)
    set_target_properties(botscript-server-gui PROPERTIES COMPILE_FLAGS "-std=c++11 -s")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
  endif()

  ################################
  # Package target
  ################################
  add_custom_target(dist
    rm -rf dist &&
    mkdir -p dist/packages &&
    cp external_lib/botscript/kv/kv.package dist/packages &&
    cp external_lib/botscript/pg/pg.package dist/packages &&
    cp botscript-server-gui.exe dist &&
    zip -r makielskis-bot.zip dist
  )
  add_dependencies(dist botscript-server-gui compile_packages)
endif()


################################
# Server executable
################################
add_executable(botscript-server-exe src/main.cc)
if(MSVC)
  target_link_libraries(botscript-server-exe botscriptserver_lib)
  set(_def
    _VARIADIC_MAX=10
    _WIN32_WINNT=0x0501
  )
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_DEFINITIONS "${_def}")
elseif(MINGW)
  target_link_libraries(botscript-server-exe
    -Wl,-Bstatic
      botscriptserver_lib
      ws2_32
      mswsock
    -static-libgcc
    -static-libstdc++
  )

  ### Compiler Definitions
  set(_def
    BOOST_THREAD_USE_LIB
    WINVER=0x0501
    _WIN32_WINNT=0x0501
    _WIN32_IE=0x0501
  )
  set_target_properties(bs PROPERTIES COMPILE_DEFINITIONS "${_def}")

  ### Compiler Flags
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_FLAGS "-std=c++11 -s")
else()
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_FLAGS "-Wall -Wextra -std=c++11")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(botscript-server-exe botscriptserver_lib)
  else()
    target_link_libraries(botscript-server-exe botscriptserver_lib rt -pthread)
  endif()
endif()


################################
# Unit Tests
################################
if (NOT MSVC AND NOT  MINGW)
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
    test/user_bot_config_test.cpp
    test/multi_index_test.cpp
  )

  if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(botscript-server-tests gtest gtest_main botscriptserver_lib)
  else()
    target_link_libraries(botscript-server-tests gtest gtest_main botscriptserver_lib -pthread)
  endif()
endif()