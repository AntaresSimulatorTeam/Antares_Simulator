#!/bin/bash

if [ $# -eq 0 ]
then
    # No arguments: format all
    SOURCE_DIRS="analyzer/ ext/ internet/ libs/ solver/ tools/ ui/"
    SOURCE_FILES=$(find $SOURCE_DIRS -type f \( -name "*.h" -or -name "*.cpp" -or -name "*.h" -or -name "*.hxx" -or -name "*.hpp" -or -name "*.cxx" -or -name "*.c" \) -and -not \( -iname "sqlite3.c" -or -name "*.h.generator.hpp" -or -name "*.hxx.generator.hpp" -or -name "*.generator.commonstypes.hpp" \))
else
    # Format files provided as arguments
    SOURCE_FILES="$@"
fi

# Remove ^M, etc.
if ! [ -x "$(command -v dos2unix)" ]; then
    echo 'Warning: dos2unix is not installed. Skipping' >&2
else
    echo "$SOURCE_FILES" | xargs dos2unix
fi

if ! [ -x "$(command -v clang-format-10)" ]; then
    echo 'Warning: clang-format-10 is not installed. Skipping' >&2
else
    echo "$SOURCE_FILES" | xargs clang-format-10 -i --verbose
fi
