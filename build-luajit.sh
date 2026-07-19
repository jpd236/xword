#!/bin/bash
# Script to build and install LuaJIT.

set -e

LUAJIT_VERSION=$1

if [[ -z "$LUAJIT_VERSION" ]]; then
  echo "First argument must be set to the LuaJIT version"
  exit 1
fi

INSTALL_PATH="$HOME/LuaJIT-$LUAJIT_VERSION"
mkdir -p $INSTALL_PATH

echo "Building LuaJIT version $LUAJIT_VERSION"

if [[ "$OSTYPE" == "darwin"* ]]; then
  BUILD_COMMAND="export MACOSX_DEPLOYMENT_TARGET=10.10 && \
                 make clean && \
                 make CC='clang -arch x86_64' && \
                 mv libluajit.a libluajit-x86_64.a && \
                 export MACOSX_DEPLOYMENT_TARGET=10.10 && \
                 make clean && \
                 make CC='clang -arch arm64' HOST_CC='clang' && \
                 mv libluajit.a libluajit-arm64.a && \
                 lipo -create -output libluajit.a libluajit-x86_64.a libluajit-arm64.a"
  INSTALL_COMMAND="cp libluajit.a $INSTALL_PATH/lib/liblua51.a"
  LIBS_DIR="macosx"
elif [[ "$OSTYPE" == "linux"* ]]; then
  BUILD_COMMAND="make CC='gcc -fPIC'"
  INSTALL_COMMAND="cp libluajit.a $INSTALL_PATH/lib/liblua51.a"
  LIBS_DIR="linux"
else  # Windows
  BUILD_COMMAND="./msvcbuild.bat"
  INSTALL_COMMAND="cp lua51.{dll,exp,lib} $INSTALL_PATH/lib/"
  LIBS_DIR="windows"
fi

# Check to see if the cache directory is empty
if [ ! -d "$INSTALL_PATH/lib" ]; then
  git clone https://github.com/LuaJIT/LuaJIT.git
  cd LuaJIT
  git checkout $LUAJIT_VERSION
  cd src
  eval $BUILD_COMMAND
  mkdir -p $INSTALL_PATH/lib
  eval $INSTALL_COMMAND
  cd ../../
else
  echo "Using cached directory."
fi

mkdir -p deps/luajit/lib/$LIBS_DIR/
cp -a $INSTALL_PATH/lib/* deps/luajit/lib/$LIBS_DIR/
