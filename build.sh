#!/bin/bash

# Clean build directory
rm -rf build
mkdir -p build
cd build

# Configure with proper compiler settings
cmake -DCMAKE_C_COMPILER=/usr/bin/clang \
      -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
      -DCMAKE_BUILD_TYPE=Debug ..

# Build
make

echo "Build complete! Run with: ./MyQtApp"