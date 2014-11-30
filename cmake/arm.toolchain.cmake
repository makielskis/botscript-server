# CMake toolchain file for building ARM software

SET(HOME_DIR $ENV{HOME})

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)

# specify the cross compiler
SET(CMAKE_C_COMPILER   ${HOME_DIR}/x-tools/arm-rpi-linux-gnueabi/bin/arm-rpi-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER ${HOME_DIR}/x-tools/arm-rpi-linux-gnueabi/bin/arm-rpi-linux-gnueabi-g++)
SET(CMAKE_STRIP ${HOME_DIR}/x-tools/arm-rpi-linux-gnueabi/bin/arm-rpi-linux-gnueabi-strip)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  ${HOME_DIR}/x-tools/arm-rpi-linux-gnueabi/arm-rpi-linux-gnueabi/sysroot)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
