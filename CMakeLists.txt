set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)

cmake_minimum_required(VERSION 2.6)
project(botscript-server)


################################
# BotScript Server Version
################################
set(botscript_server_gui_address \"http://5.45.107.68/ui/0.0.8\")
set(botscript_server_major 0)
set(botscript_server_minor 2)
set(botscript_server_patch 2)
configure_file (
  "src/botscript_server_version.h.in"
  "${PROJECT_SOURCE_DIR}/src/botscript_server_version.h"
)
set(botscript_server_version ${botscript_server_major}.${botscript_server_minor}.${botscript_server_patch})


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
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS
  system
  thread
  date_time
  filesystem
  iostreams
  regex
  random
  program_options
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
file(GLOB_RECURSE botscriptserver_lib_files "src/*.cc")
list(REMOVE_ITEM botscriptserver_lib_files ${gui_files})
list(REMOVE_ITEM botscriptserver_lib_files ${exe_files})
list(REMOVE_ITEM botscriptserver_lib_files ${jni_files})

add_library(botscriptserver_lib STATIC ${botscriptserver_lib_files})
if(MSVC)
  target_link_libraries(botscriptserver_lib bs dust ${Boost_LIBRARIES})
  set(_def
    _VARIADIC_MAX=10
    _WIN32_WINNT=0x0501
  )
  set_target_properties(botscriptserver_lib PROPERTIES COMPILE_DEFINITIONS "${_def}")
else()
  target_link_libraries(botscriptserver_lib bs dust ${Boost_LIBRARIES} z)
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
      target_link_libraries(botscriptserver_lib bs dust ${Boost_LIBRARIES} z)
    else()
      target_link_libraries(botscriptserver_lib bs dust ${Boost_LIBRARIES} z rt  -pthread)
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
  set(foldername makielskisbot-${botscript_server_version})
  set(packagename makielskisbot-win-${botscript_server_version}.zip)
  add_custom_target(package
    rm -rf ${foldername} &&
    mkdir -p ${foldername}/packages &&
    cp external_lib/botscript/kv/kv.package ${foldername}/packages &&
    cp external_lib/botscript/pg/pg.package ${foldername}/packages &&
    cp botscript-server-gui.exe ${foldername}/makielskisbot.exe &&
    cp ${CMAKE_CURRENT_LIST_DIR}/win_res/config.ini ${foldername} &&
    zip -r -9 ${packagename} ${foldername}
  )
  add_dependencies(package botscript-server-gui compile_packages)

  ################################
  # Deploy Upload
  ################################
  add_custom_target(release
    scp ${packagename} perry@5.45.107.68:/var/www/dl/
  )
  add_dependencies(release package)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Tell CMake to run moc when necessary:
  set(CMAKE_AUTOMOC ON)

  # As moc files are generated in the binary dir, tell CMake
  # to always look for includes there:
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # Widgets finds its own dependencies.
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Core REQUIRED)

  set(systray_SRCS src/qtgui/res/systray.qrc)
  qt5_add_resources(systray_RCC_SRCS ${systray_SRCS})

  add_executable(botscript-server-gui
    ${GUI_TYPE}
    ${systray_RCC_SRCS}
    src/qtgui/main.cpp
    src/qtgui/Window.cpp
    src/qtgui/BotThread.cpp
  )

  target_link_libraries(botscript-server-gui botscriptserver_lib)
  set_target_properties(botscript-server-gui PROPERTIES COMPILE_FLAGS "-std=c++11")

  qt5_use_modules(botscript-server-gui Widgets)

  ################################
  # Package target
  ################################
  set(foldername makielskisbot.app)
  set(packagename makielskisbot-mac-${botscript_server_version}.zip)
  add_custom_target(package
    rm -rf ${foldername} &&
    mkdir -p ${foldername}/packages &&
    cp ${CMAKE_CURRENT_LIST_DIR}/mac_os_res/Info.plist ${foldername} &&
    cp ${CMAKE_CURRENT_LIST_DIR}/mac_os_res/makielskisbotIcon.icns ${foldername} &&
    cp external_lib/botscript/kv/kv.package ${foldername}/packages &&
    cp external_lib/botscript/pg/pg.package ${foldername}/packages &&
    cp botscript-server-gui ${foldername}/makielskisbot &&
    zip -r -9 ${packagename} ${foldername}
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
# Unit Tests
################################
if (NOT MSVC AND NOT  MINGW)
  if (NOT TARGET gtest)
    add_subdirectory(test/gtest-1.7.0)
  endif()

  enable_testing()
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
  set_target_properties(botscript-server-tests PROPERTIES COMPILE_FLAGS "-std=c++11")

  add_test(botscript-server-tests botscript-server-tests)
endif()
