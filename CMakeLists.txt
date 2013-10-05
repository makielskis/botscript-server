# Compile and run release version:
# mkdir build
# cd build
# cmake -DCMAKE_BUILD_TYPE=Release ..
# cd ..
# ./build/botscript

# Compile and run debug version
# mkdir build
# cd build
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# cd ..
# ./build/botscript

set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxx_flag_overrides.cmake)


cmake_minimum_required (VERSION 2.8.8)
project(botscript-server)


################################
# Dependencies
################################
add_subdirectory(external_lib/botscript)
add_subdirectory(external_lib/kyotocabinet-1.2.76)


################################
# Boost
################################

# Windows compile command:
# b2 --with-system --with-thread --with-date_time --with-random --with-chrono --with-regex --with-filesystem --with-iostreams runtime-link=static link=static -s ZLIB_SOURCE=C:\zlib-1.2.8

if(MSVC)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/boost_1_52_0")
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/boost_1_52_0/stage/lib")
  set(Boost_USE_STATIC_RUNTIME ON)
endif()

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
# Compile flags
################################
if(MSVC)
  # VS2012 doesn't support correctly the tuples yet
  add_definitions(/D _VARIADIC_MAX=10)
  add_definitions(/D _WIN32_WINNT=0x0501)
elseif(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -s")
  add_definitions(-DBOOST_THREAD_USE_LIB -D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -D_WIN32_IE=0x0501)
else()
  # GCC -> use c++11 standard.
  add_definitions("-std=c++11 -fPIC")

  # Show all warnings.
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
endif()


################################
# Include Paths
################################
include_directories("external_lib/websocketpp")
include_directories("external_lib/botscript/src")
include_directories("external_lib/botscript/external_lib/rapidjson/include")
include_directories("external_lib/botscript/external_lib/pugixml-1.0/src")
include_directories("external_lib/botscript/external_lib/lua-5.2.0/src")
include_directories("external_lib/kyotocabinet-1.2.76")


################################
# Object library
################################
add_library(botscriptserver_lib STATIC
  src/error.cc
  src/storage/file_config_store.cc
  src/messages/login_msg.cc
  src/messages/register_msg.cc
  src/messages/user/user_msg.cc
  src/messages/user/bot/create_bot_msg.cc
  src/messages/user/bot/delete_bot_msg.cc
  src/messages/user/bot/execute_bot_msg.cc
  src/messages/user/bot/reactivate_bot_msg.cc
  src/messages/user/update/delete_update_msg.cc
  src/messages/user/update/email_update_msg.cc
  src/messages/user/update/password_update_msg.cc
  src/messages/outgoing/success_msg.cc
  src/messages/outgoing/failure_msg.cc
  src/messages/outgoing/packages_msg.cc
  src/messages/outgoing/session_msg.cc
  src/messages/outgoing/account_msg.cc
  src/messages/outgoing/bots_msg.cc
  src/messages/outgoing/update_msg.cc
)


################################
# Main executable
################################
add_executable(botscript-server-exe
  src/ws_server.cc
  src/bot_manager.cc
  src/storage/db_user_store.cc
  src/storage/db_config_store.cc
  src/main.cc
)
if(MSVC)
  target_link_libraries(botscript-server-exe botscriptserver_lib bs ${Boost_LIBRARIES} kyotocabinet)
elseif(MINGW)
  target_link_libraries(botscript-server-exe botscriptserver_lib bs ${Boost_LIBRARIES} kyotocabinet -Wl,-Bstatic z ws2_32 mswsock -static-libgcc -static-libstdc++)
else()
  target_link_libraries(botscript-server-exe botscriptserver_lib bs ${Boost_LIBRARIES} kyotocabinet rt -pthread)
endif()


################################
# GUI executable
################################
if(MSVC OR MINGW)
  add_executable(botscript-server-gui
    WIN32
    src/ws_server.cc
    src/bot_manager.cc
    src/storage/desktop_user_store.cc
    src/win32gui/TrayIcon.cpp
    src/win32gui/main.cc
    src/win32gui/res/bot.rc
  )
  set_target_properties(botscript-server-gui PROPERTIES COMPILE_FLAGS "-DDESKTOP")
  if(MSVC)
    target_link_libraries(botscript-server-gui botscriptserver_lib bs ${Boost_LIBRARIES})
  elseif(MINGW)
    target_link_libraries(botscript-server-gui botscriptserver_lib bs ${Boost_LIBRARIES} -Wl,-Bstatic z ws2_32 mswsock -static-libgcc -static-libstdc++)
  endif()
endif()


################################
# Unit Tests
################################
if (NOT TARGET gtest)
  add_subdirectory(test/gtest-1.6.0)
  enable_testing()
endif()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(botscript-server-tests
  src/storage/db_user_store.cc
  src/storage/db_config_store.cc
  src/bot_manager.cc
  test/db_user_store_test.cpp
  test/db_config_store_test.cpp
  test/error_test.cpp
  test/rapid_json_exception_test.cpp
  test/outgoing_messages_test.cpp
  test/bot_manager_test.cpp
)

target_link_libraries(botscript-server-tests botscriptserver_lib gtest gtest_main bs ${Boost_LIBRARIES} kyotocabinet rt -pthread)
add_test(botscript-server-tests botscript-server-tests)
