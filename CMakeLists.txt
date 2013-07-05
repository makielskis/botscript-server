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


cmake_minimum_required (VERSION 2.8.8)
project(botscript-server)


################################
# Dependencies
################################
add_subdirectory(external_lib/botscript)
add_subdirectory(external_lib/websocketpp)
add_subdirectory(external_lib/kyotocabinet-1.2.76)


################################
# Boost
################################
if(MSVC)
  set(CMAKE_INCLUDE_PATH "C:/boost_1_52_0")
  set(CMAKE_LIBRARY_PATH "C:/boost_1_52_0/stage/lib") 
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS
  system
  thread
  date_time
  filesystem
  iostreams
  regex
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
else()
  # GCC -> use c++11 standard.
  add_definitions("-std=c++11 -fPIC")

  # Show all warnings.
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  set(CMAKE_CSS_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
endif()


################################
# Include Paths
################################
include_directories("external_lib/rapidjson-r64/include")
include_directories("external_lib/websocketpp/src")
include_directories("external_lib/botscript/src")
include_directories("external_lib/botscript/external_lib/pugixml-1.0/src")
include_directories("external_lib/botscript/external_lib/lua-5.2.0/src")
include_directories("external_lib/kyotocabinet-1.2.76")
include_directories("/usr/lib/jvm/java-7-openjdk-amd64/include/")


################################
# Object library
################################
add_library(botscriptserver_obj_lib OBJECT
  src/botserver_handler.cc
  src/message.cc
  src/service.cc
)


################################
# Shared Library
################################
add_library(botscript-server SHARED
  $<TARGET_OBJECTS:botscriptserver_obj_lib>
  src/net_makielski_botscript_BotscriptServer.cpp
)
if(MSVC)
  target_link_libraries(botscript-server bs ${Boost_LIBRARIES} websocketpp kyotocabinet)
else()
  target_link_libraries(botscript-server bs ${Boost_LIBRARIES} websocketpp kyotocabinet rt -pthread)
endif()


################################
# Main executable
################################
add_executable(botscript-server-exe
  $<TARGET_OBJECTS:botscriptserver_obj_lib>
  src/main.cc
)
if(MSVC)
  target_link_libraries(botscript-server-exe bs ${Boost_LIBRARIES} websocketpp kyotocabinet)
else()
  target_link_libraries(botscript-server-exe bs ${Boost_LIBRARIES} websocketpp kyotocabinet rt -pthread)
endif()


################################
# GUI executable
################################
if(WITH_GUI)
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

  if(WIN32)
	set(GUI_TYPE WIN32)
  endif(WIN32)

  add_executable(botscript-server-gui
    ${GUI_TYPE}
    ${systray_RCC_SRCS}
    $<TARGET_OBJECTS:botscriptserver_obj_lib>
    src/qtgui/main.cpp
    src/qtgui/Window.cpp
    src/qtgui/BotThread.cpp
  )

  if(MSVC)
    #set_target_properties(botscript-server-gui PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    #set_target_properties(botscript-server-gui PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")
	add_definitions(/D WIN32_LEAN_AND_MEAN)
    target_link_libraries(botscript-server-gui
      Qt5::WinMain
      ${Qt5Widgets_LIBRARIES} bs ${Boost_LIBRARIES} websocketpp kyotocabinet
    )
  else()
    target_link_libraries(botscript-server-gui bs ${Boost_LIBRARIES} websocketpp kyotocabinet rt -pthread)
  endif()

  qt5_use_modules(botscript-server-gui Widgets)
endif()
