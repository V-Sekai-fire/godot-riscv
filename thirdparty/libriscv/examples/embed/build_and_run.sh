#!/bin/bash
set -e

mkdir -p .build
pushd .build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFETCHCONTENT_UPDATES_DISCONNECTED=ON
make -j4
popd

# fib(64) == 10610209857723
./.build/example fib.rv64.elf 64
