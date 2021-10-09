#!/bin/bash
PROJECT_DIR=$1
OUT_FILE=$2
cd $PROJECT_DIR;
cat CMakeLists.txt | sed -n '/^# HEADER-BEGIN/,${p;/^# HEADER-END/q}' > $OUT_FILE;
cat src/CMakeLists.txt >> $OUT_FILE;