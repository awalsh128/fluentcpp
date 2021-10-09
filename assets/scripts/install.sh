#!/bin/bash

if [ "$EUID" -ne 0 ]; then
  echo "Installation must be ran as root to install to local library and include directories."
  exit 1
fi

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Debug --target fluentcpp
cmake --install

echo "Cleaning up build artifacts."
cd ..
rm -fr build

echo "Done"
