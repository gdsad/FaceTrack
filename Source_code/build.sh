#!/bin/bash
echo "Configuring..."
cmake -S . -B ./_out/build
echo "Building targets..."
cmake --build ./_out/build -j8
