set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)

cmake_minimum_required(VERSION 2.6)
project(botscript-server)


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
  COMMAND git describe --tags --dirty=-dirty
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


################################
# Dependencies
################################
add_subdirectory(external_lib/botscript)
add_subdirectory(external_lib/dust-server)


################################
# Boost
################################
set(_boostlibs
  system
  thread
  filesystem
  iostreams
  regex
  program_options
)
if(NOT ANDROID)
  list(APPEND _boostlibs
    date_time
    random
  )
endif()
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS ${_boostlibs} REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
  message("No Boost found.")
endif(Boost_FOUND)


################################
# Include Paths
################################
include_directories("external_lib/websocketpp")
include_directories("external_lib/cereal/include")
include_directories("external_lib/botscript/src")
include_directories("external_lib/botscript/external_lib/rapidjson/include")
include_directories("external_lib/botscript/external_lib/pugixml-1.2/src")
include_directories("external_lib/botscript/external_lib/lua-5.2/src")
include_directories("external_lib/dust-server/external_lib/dust/include")
include_directories("external_lib/dust-server/external_lib/http_server")
include_directories("external_lib/dust-server/include")


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
if(MSVC)
  target_link_libraries(botscriptserver_lib bs dust ${Boost_LIBRARIES})
  set(_def
    _VARIADIC_MAX=10
    _WIN32_WINNT=0x0501
  )
  set_target_properties(botscriptserver_lib PROPERTIES COMPILE_DEFINITIONS "${_def}")
else()
  if(MINGW)
    target_link_libraries(botscriptserver_lib
      ${Boost_LIBRARIES}
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
      target_link_libraries(botscriptserver_lib bs dust ${Boost_LIBRARIES} z)
    elseif(ANDROID)
      target_link_libraries(botscriptserver_lib ${Boost_LIBRARIES} bs dust z)
    else()
      target_link_libraries(botscriptserver_lib ${Boost_LIBRARIES} bs dust z rt -pthread)
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
  #include_directories(/System//Library/Frameworks/JavaVM.framework/Versions/A/Headers/)
  #include_directories(/usr/lib/jvm/java/include/)
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
message(${c_api_files})
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
      _WIN32_WINNT=0x0501
      WINVER=0x0501
      _WIN32_IE=0x0501
    )
  else() # MSVC
    set(_def
      _VARIADIC_MAX=10
      _WIN32_WINNT=0x0501
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
  set(foldername makielskisbot-${botscript_server_git_tag})
  set(packagename makielskisbot-win-${botscript_server_git_tag}.zip)
  add_custom_target(package
    rm -rf ${foldername} &&
    mkdir -p ${foldername}/packages &&
    cp external_lib/botscript/kv/kv.package ${foldername}/packages &&
    cp external_lib/botscript/pg/pg.package ${foldername}/packages &&
    cp botscript-server-gui.exe ${foldername}/makielskisbot.exe &&
    cp ${CMAKE_CURRENT_LIST_DIR}/win_res/config.ini ${foldername} &&
    zip -r ${packagename} ${foldername}
  )
  add_dependencies(package botscript-server-gui compile_packages)

  ################################
  # Deploy Upload
  ################################
  add_custom_target(release
    scp ${packagename} perry@5.45.107.68:/var/www/dl/
  )
  add_dependencies(release package)
endif()


################################
# Server executable
################################
add_executable(botscript-server-exe src/main.cc)

if(MSVC)
  target_link_libraries(botscript-server-exe botscriptserver_lib dust-server)
  set(_def
    _VARIADIC_MAX=10
    _WIN32_WINNT=0x0501
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
    WINVER=0x0501
    _WIN32_WINNT=0x0501
    _WIN32_IE=0x0501
  )
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_DEFINITIONS "${_def}")

  ### Compiler Flags
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_FLAGS "-std=c++11 -s")
else()
  set_target_properties(botscript-server-exe PROPERTIES COMPILE_FLAGS "-Wall -Wextra -std=c++11")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(botscript-server-exe botscriptserver_lib dust-server)
  else()
    target_link_libraries(botscript-server-exe botscriptserver_lib dust-server rt -pthread)
  endif()
endif()

################################
# Server package target
################################
set(server-foldername makielskisbot-server-${botscript_server_git_tag})
set(server-packagename makielskisbot-server-${botscript_server_git_tag}.zip)
add_custom_target(server-package
  rm -rf ${server-foldername} &&
  mkdir -p ${server-foldername}/packages &&
  cp external_lib/botscript/kv/kv.package ${server-foldername}/packages &&
  cp external_lib/botscript/pg/pg.package ${server-foldername}/packages &&
  cp botscript-server-exe ${server-foldername}/makielskisbot &&
  zip -r -9 ${server-packagename} ${server-foldername}
)
add_dependencies(server-package botscript-server-exe compile_packages)


################################
# Unit Tests
################################
if (NOT MSVC AND NOT  MINGW)
  if (NOT TARGET gtest)
    add_subdirectory(test/gtest-1.7.0)
  endif()

  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

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
    target_link_libraries(botscript-server-tests gtest gtest_main botscriptserver_lib -pthread)
  endif()
  set_target_properties(botscript-server-tests PROPERTIES COMPILE_FLAGS "-std=c++11")

  add_test(botscript-server-tests botscript-server-tests)
endif()
