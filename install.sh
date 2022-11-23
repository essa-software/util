#!/bin/bash

if [ ! -e build ]; then
    mkdir build
fi
cd build

# FIXME: Don't hardcode this path, instead find a better CMake-supported
# way to check if FMT is installed.
test -r /usr/lib/cmake/fmt/fmt-config.cmake
FMT_FOUND=$?

if [ $FMT_FOUND -eq 1 ]; then
    git clone https://github.com/fmtlib/fmt fmt --depth 1 --branch 9.1.0
    mkdir fmt/build
    cd fmt/build
    cmake .. -GNinja -DBUILD_SHARED_LIBS=1 -DFMT_TEST=0
    ninja
    sudo ninja install
    cd ../..
fi

cmake .. -GNinja $CMAKE_OPTIONS
ninja
sudo ninja install
