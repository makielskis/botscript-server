# How to cross compile botscript-server

We use  MXE (M cross environment) as our cross compiling toolchain. It is available at http://mxe.cc.

### Install the requirements

#### Debian/Ubuntu:

    apt-get install autoconf automake bash bison bzip2 \
                    cmake flex gettext git g++ intltool \
                    libffi-dev libtool libltdl-dev libssl-dev \
                    libxml-parser-perl make openssl patch perl \
                    pkg-config scons sed unzip wget xz-utils

#### Fedora:

    yum install autoconf automake bash bison bzip2 cmake \
                flex gcc-c++ gettext git intltool make sed \
                libffi-devel libtool openssl-devel patch perl pkgconfig \
                scons unzip wget xz

### Download MXE

    cd ~/
    git clone -b stable https://github.com/mxe/mxe.git

### Install/compile compiler and libraries

    make -j4 boost libgnurx

### Setup system

Add the following to your ~/.bashrc :

    export MXE_ROOT=~/mxe
    alias mxe-cmake='cmake -DCMAKE_TOOLCHAIN_FILE=$MXE_ROOT/usr/i686-pc-mingw32/share/cmake/mxe-conf.cmake -DBoost_THREADAPI=win32'

### Compile botscript-server

    cd [botscript-server-directory]
    mkdir build_win
    cd build_win
    mxe-cmake ..
    make botscript-server-exe botscript-server-gui

### You're done.

# MinGW command to compile boost on Windows

    NO_BZIP2=1 ./b2 --with-system --with-thread --with-regex --with-iostreams --with-filesystem --with-chrono --with-date_time --with-random variant=release link=static threading=multi runtime-link=static --toolset=gcc-mingw64 define=BOOST_USE_WINDOWS_H threadapi=win32 target-os=windows -s ZLIB_SOURCE=[PATH_TO_ZLIB]

# Generating the JNI header

In `src/net/makielski/botscript`:  

    javac BotscriptServer.java


In `src` javac BotscriptServer.java`:  

    javah -jni net.makielski.botscript.BotscriptServer

# Cross-Compiling for ARM Platform

Install `crosstool-ng` dependencies:

    apt-get install gawk bison flex gperf cvs texinfo automake \
                       libtool ncurses-dev g++ subversion python-dev \
                       libexpat1-dev cmake


### Download and compile `crosstool-ng`

    wget http://crosstool-ng.org/download/crosstool-ng/crosstool-ng-1.20.0.tar.bz2
    tar xf crosstool-ng-1.20.0.tar.bz2
    cd crosstool-ng-1.20.0
    ./configure --prefix=/path/to/installation
    make
    make install
    export PATH=$PATH:/path/to/installation/bin


### Create toolchain

    mkdir /path/to/toolchain-config
    cd /path/to/toolchain-config
    cp /path/to/botscript-server-repo/crosstool-ng.config .config
    ct-ng menuconfig  // load configuration file (".config")
    ct-ng build

**Settings (to reproduce the configuration)**

- Paths & Misc: 
    - Check "Try features marked as EXPERIMENTAL"
    - Set "Prefix directory" to whereever you want the finished toolchain to be placed (e.g. /home/<yourname>/crosscompile)
    - Set "Number of parallel jobs" to be the number of processor cores in your system x1.5
- Target options:
    - Set "Target architecture" to "ARM"
    - Set "Endianness" to "Little endian"
    - Set "Bitness" to "32-bit"
    - Set "Architecture level" to "armv6zk"
    - Set "Emit assembly for CPU" to "arm1176jzf-s"
    - Set "Tune for CPU" to "arm1176jzf-s"
    - Set "Use specific FPU" to "vfp"
    - Set "Floating point" to "hardware (FPU)"
    - Set "Default instruction set mode" to "arm"
    - Check "Use EABI"
- General toolchain:
    - Set "Tuple's vendor string" to "rpi" - you can choose whatever you like here and it will appear in the compiler filenames.
- Operating system:
    - Set "Target OS" to "linux"
    - Set "Linux kernel version" to match that running on the RPI (it was tested with the 3.6 branch patched to version 3.6.3)
- Binary utilities:
    - Set "Binary format" to "ELF"
    - Set "binutils version" to "2.22"
- C compiler:
    - Check "Show linaro versions"
    - Set "gcc version" to "linaro-4.7-2012.10"
    - Check "C++"
    - Set "gcc extra config" to "--with-float=hard"    (Note: see "Discussion" page)
    - Check "Link libstdc++ statically into gcc binary"
- C-library:
    - Set "C library" to "eglibc"
    - Set "eglibc version" to "2_13"

### Build zlib

(replace `--prefix=...` path with your sysroot path of the `crosstool-ng` tool-chain)

    wget http://zlib.net/zlib-1.2.8.tar.gz
    tar xf zlib-1.2.8.tar.gz
    cd zlib-1.2.8
    CC=/root/x-tools/arm-rpi-linux-gnueabi/bin/arm-rpi-linux-gnueabi-gcc ./configure --prefix=/root/x-tools/arm-rpi-linux-gnueabi/arm-rpi-linux-gnueabi/sysroot
    make
    make install


### Build Boost

Download Boost (i.e. version 1.49.0) from [boost.org](http://boost.org).
Change the path to your tool-chain accordingly.

    tar xf boost-1.49.0.tar.bz2
    cd boost-1.49.0
    ./bootstrap.sh
    echo "using gcc : arm : /root/x-tools/arm-rpi-linux-gnueabi/bin/arm-rpi-linux-gnueabi-g++ ;" > ~/user-config.jam
    NO_BZIP2=1 ./b2 install --layout=system --with-random --with-program_options --with-date_time --with-chrono --with-system --with-thread --with-regex --with-iostreams --with-filesystem variant=release link=static threading=multi toolset=gcc-arm --prefix=/root/x-tools/arm-rpi-linux-gnueabi/arm-rpi-linux-gnueabi/sysroot -s ZLIB_SOURCE=$HOME/zlib-1.2.8

### Build Botscript Server


Install Dependencies (to compile luac for the host machine):

    apt-get install build-essential gcc g++ libc6-dev-i386 g++-multilib


Build Botscript Server:

    cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm.toolchain.cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DSTATIC_PACKAGES:bool=ON -DCROSS_ARM:bool=ON ..
    make botscript-server-exe
    

