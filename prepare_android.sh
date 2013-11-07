#!/bin/bash

# goto and save path to this scripts location in filesystem
ORIGINAL_DIR=`pwd`;
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )";
cd $DIR;

function echo_bold {
  local bold=`tput bold`;
  local normal=`tput sgr0`;
  echo -e "\n::: ${bold}$1${normal}\n";
}

function prepareNDK() {
  # Don't do this twice
  if  [ -z $NDK ]; then
    # is 'ndk-build' available in PATH?
    if hash ndk-build 2>/dev/null; then
      # get location of 'ndk-build'
      local path=$(command -v ndk-build);
      NDK=${path%/*};
      echo_bold "NDK found: '$NDK'";
    else
      # cannot continue without NDK
      echo_bold "Cannot find NDK. Make sure it is in your PATH.";
      exit 1;
    fi
  fi
}

function prepareToolchain() {
  # is compiler in PATH?
  if hash arm-linux-androideabi-g++ 2>/dev/null; then
    # get path to compiler
    local path=$(command -v arm-linux-androideabi-g++);
    TOOLCHAIN=${path%/*};
    echo_bold "Toolchain found: '$TOOLCHAIN'";
  else
    echo_bold "Toolchain not found. Creating one.";

    # make sure NDK is set
    prepareNDK;

    # generate a toolchain with the NDK
    $NDK/build/tools/make-standalone-toolchain.sh --toolchain=arm-linux-androideabi-4.8 --llvm-version=3.3 --install-dir=$DIR/toolchain;

    # save path to toolchain
    TOOLCHAIN=$DIR/toolchain/bin;
    export PATH=$PATH:$TOOLCHAIN;

    echo_bold "You perhaps want to add 'export PATH=\$PATH:$TOOLCHAIN' to your .bashrc.";
  fi

  # set compiler environment variables
  export CXX=arm-linux-androideabi-g++;
  export CC=arm-linux-androideabi-gcc;
}

function prepareBoost() {
  # are boost libs and headers in there expected location?
  if [ -d $DIR/android-boost/build/lib -a -d $DIR/android-boost/build/include/boost-1_53/ ]; then
    # we're fine.
    echo_bold "Boost seems ready."
  else
    # boost is missing so get it
    echo_bold "Boost not found. Compiling it.";

    # NDK is needed
    prepareNDK;

    # clean up any previous installation
    rm -rf $DIR/android-boost;

    # get a known working copy of the boost for android scripts
    git clone https://github.com/MysticTreeGames/Boost-for-Android.git android-boost;
    cd android-boost; # always remember to go back to $DIR
    git checkout 02f7e8d384ad5cdca65528ddae12ea53cb897c3a; # latest checked commit; uses boost 1.53

    # build boost
    ./build-android.sh --with-libraries=system,thread,date_time,chrono,regex,filesystem,iostreams,random $NDK;

    # go back to the default dir
    cd $DIR;
  fi

  # save include und library paths for boost
  export BOOST_INCLUDE=$DIR/android-boost/build/include/boost-1_53;
  export BOOST_LIB=$DIR/android-boost/build/lib;
}

prepareNDK;
prepareBoost;
prepareToolchain;

cd $ORIGINAL_DIR;

echo_bold "CMake Command: 'cmake -DCMAKE_INCLUDE_PATH=\$BOOST_INCLUDE -DCMAKE_LIBRARY_PATH=\$BOOST_LIBS -DBoost_COMPILER=-gcc -DANDROID:boolean=true;'";
