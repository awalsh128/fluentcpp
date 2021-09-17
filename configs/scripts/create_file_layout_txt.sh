#!/bin/bash

cd ../..

if ! `which tree`; then
  sudo apt install tree
fi
tree -I 'build|_deps|.vscode|*.code-workspace|docs/api|tree.txt' > build/tree_struct.txt