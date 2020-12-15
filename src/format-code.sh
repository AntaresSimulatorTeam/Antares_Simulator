#!/bin/bash

SOURCE_DIRS="analyzer/ ext/ internet/ libs/ solver/ tools/ ui/"

# Remove ^M, etc.
if ! [ -x "$(command -v dos2unix)" ]; then
    echo 'Warning: dos2unix is not installed. Skipping' >&2
else
    find $SOURCE_DIRS -type f \( -name "*.h" -or -name "*.cpp" -or -name "*.h" -or -name "*.hxx" -or -name "*.cxx" \) | xargs dos2unix
fi

if ! [ -x "$(command -v clang-format-10)" ]; then
    echo 'Warning: clang-format-10 is not installed. Skipping' >&2
else
    find $SOURCE_DIRS -type f \( -name "*.h" -or -name "*.cpp" -or -name "*.h" -or -name "*.hxx" -or -name "*.cxx" \) | xargs clang-format-10 -i --verbose
fi
