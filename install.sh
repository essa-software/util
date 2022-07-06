#!/bin/bash
if [ ! -e build ]; then
    mkdir build
fi
cd build
cmake .. -GNinja $CMAKE_OPTIONS
ninja
sudo ninja install
