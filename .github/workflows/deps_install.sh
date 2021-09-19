#!/bin/bash

cache_dir=$1
packages="dia doxygen doxygen-doc doxygen-gui doxygen-latex graphviz mscgen"

for dir in `ls $cache_dir`; do
  remove=true
  for package in $packages; do
    if [ $dir = $package ]; then
      remove=false
      break
    fi
  done
  [ "$remove" = true ] && sudo apt remove $dir
done

for package in $packages; do
 
  package_dir=$cache_dir/$package

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
