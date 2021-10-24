#!/bin/bash -x

PROJECT_DIR=$1
TAG=$2
NOTES=$3

cd $PROJECT_DIR

rm -fr release 2> /dev/null
mkdir release

RELEASE_PKG_DIR="release"
mkdir $RELEASE_PKG_DIR

cat CMakeLists.txt | sed -n '/^# HEADER-BEGIN/,${p;/^# HEADER-END/q}' > $RELEASE_PKG_DIR/CMakeLists.txt;
cp -f src/*.cpp release
cp -f src/*.h release
cp -f assets/scripts/install.sh release

cd release
PKG_NAME="fluentcpp-$TAG"
tar -czvf $PKG_NAME.tar.gz *
zip $PKG_NAME.zip *

cd ..
echo "**Full Changelog**: https://github.com/awalsh128/fluentcpp/commits/$TAG" > notes.md
echo $NOTES >> notes.md
