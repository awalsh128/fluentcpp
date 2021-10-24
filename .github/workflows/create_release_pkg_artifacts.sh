#!/bin/bash -x

PROJECT_DIR=$1
TAG=$2
NOTES=$3

cd $PROJECT_DIR

rm -fr release 2> /dev/null
mkdir release
mkdir release/src

cat CMakeLists.txt \
| sed -n '/^# HEADER-BEGIN/,${p;/^# HEADER-END/q}' \
> release/src/CMakeLists.txt;

cat src/CMakeLists.txt > release/src/CMakeLists.txt

cp -f src/*.cpp release/src
cp -f src/*.h release/src
cp -f assets/scripts/install.sh release/src

echo "**Full Changelog**: https://github.com/awalsh128/fluentcpp/releases/tag/$TAG\n\n" > release/notes.md
echo $NOTES >> release/notes.md

PKG_NAME="fluentcpp-$TAG"

tar -czvf release/$PKG_NAME.tar.gz -C release/src .
zip -j release/$PKG_NAME.zip release/src/*
rm -fr release/src
