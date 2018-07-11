#! /bin/sh

find -name 'Makefile' -type f -exec rm -rf {} \+
find -name 'CMakeFiles' -type d -exec rm -rf {} \+
find -name 'CMakeCache.txt' -type f -exec rm -f {} \+
find -name 'cmake_install.cmake' -type f -exec rm -f {} \+
