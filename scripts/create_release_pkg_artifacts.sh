#!/bin/bash -x

PROJECT_DIR=$1
TAG=$2
NOTES=$3

cd $PROJECT_DIR

rm -fr release 2> /dev/null
mkdir -p release/src

# Ship only what is needed to build and install the library. Tests,
# benchmarks, examples and docs are opt-in subdirectories and are left out.
cp -f CMakeLists.txt LICENSE README.md release/src
cp -rf cmake include src release/src
cp -f scripts/install.sh release/src

echo "**Full Changelog**: https://github.com/awalsh128/fluentcpp/releases/tag/$TAG\n\n" > release/notes.md
echo $NOTES >> release/notes.md

PKG_NAME="fluentcpp-$TAG"

tar -czvf release/$PKG_NAME.tar.gz -C release/src .
(cd release/src && zip -r ../$PKG_NAME.zip .)
rm -fr release/src
