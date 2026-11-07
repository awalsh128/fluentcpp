#!/bin/bash

IN_FILE=$1
OUT_FILE=$2

rm -fr $OUT_FILE

IN_DIRNAME=$(dirname $IN_FILE)
while IFS='' read line; do
  # Line contains href-include attribute.
  if [ ! -z "$(echo $line | grep "href-include")" ]; then
    # Current space width to prefix for each line.
    spaces=$(echo "$line" | awk -F\< '{print $1}')
    # Hacky way to extract attribute value.
    include_file="$IN_DIRNAME/$(echo $line | awk -F\" '{print $2}')"
    while IFS='' read include_line; do
      # Output include file lines in its place.
      echo "$spaces""$include_line" >> $OUT_FILE
    done < $include_file
  else
    # ... else just output the file line.
    echo "$line" >> $OUT_FILE
  fi    
done < $IN_FILE