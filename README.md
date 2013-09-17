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

