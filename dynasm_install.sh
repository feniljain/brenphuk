#!/bin/bash

set -e

# exit if minilua already exists
if [ ! -f minilua ]; then
  echo "minilua file does not exist, cloning and building"
else
	exit 0
fi

git clone https://github.com/corsix/dynasm-doc.git
cd dynasm-doc
git submodule update --init
gcc -o minilua luajit-2.0/src/host/minilua.c -lm

cp minilua ../
cp luajit-2.0/dynasm/dasm_proto.h ../include/
cp luajit-2.0/dynasm/dasm_x86.h ../include/
