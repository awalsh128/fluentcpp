#!/bin/bash

CACHE_DIR=$1
PACKAGES="dia doxygen doxygen-doc doxygen-gui doxygen-latex graphviz mscgen"

for dir in `ls $CACHE_DIR`; do
  REMOVE=true
  for package in $PACKAGES; do
    if [ $dir = $package ]; then
      REMOVE=false
      break
    fi
  done
  [ "$REMOVE" = true ] && sudo apt REMOVE $dir
done

for package in $PACKAGES; do
  
  package_dir=$CACHE_DIR/$package
  
  if [ -d $package_dir ]; then
    
    echo "Restoring $package from cache $package_dir..."
    sudo cp --verbose --force --recursive $package_dir/* /
    sudp apt-get -y update $package
  else
    
    echo "Clean install $package and caching to $package_dir..."
    sudo apt-get -y install $package
    
    echo "Caching $package to $package_dir..."
    mkdir -p $package_dir
    # Pipe all package files (no folders) to copy command.
    sudo dpkg -L $package |
    while IFS= read -r f; do
      if test -f $f; then echo $f; fi;
    done |
    xargs cp -p -t $package_dir
  fi
  
done
