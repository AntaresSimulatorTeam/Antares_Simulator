#!/bin/sh

root=`dirname "${0}"`

find "${root}/../src" '(' \
		-name 'cmake_install.cmake' \
		-or -name 'CMakeCache.txt' \
		-or -name 'CMakeFiles' \
		-or -name 'Makefile' \
		-or -name '*.xcodeproj' \
		-or -name 'CMakeScripts' \
		-or -name '*.o' \
		-or -name '.*.sw*' \
	')' -exec echo ' * Removing `' '{}' '`' \; -exec rm -rf '{}' \;

# yuni-config
rm -f "${root}/../src/tools/yuni-config/yuni-config"
