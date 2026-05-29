#!/bin/bash

set -euo pipefail

# Si clang-format est absent ou a une version différente de REQUIRED_CLANG_FORMAT_VERSION -> erreur.

REQUIRED_CLANG_FORMAT_VERSION="18.1.3"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if ! command -v clang-format >/dev/null 2>&1; then
  echo "Error: clang-format not found. Please install clang-format version $REQUIRED_CLANG_FORMAT_VERSION." >&2
  exit 1
fi

CLANG_FORMAT_VERSION=$(clang-format --version 2>&1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || true)
if [ -z "$CLANG_FORMAT_VERSION" ] || [ "$CLANG_FORMAT_VERSION" != "$REQUIRED_CLANG_FORMAT_VERSION" ]; then
  echo "Error: found clang-format version ${CLANG_FORMAT_VERSION:-unknown} but required is $REQUIRED_CLANG_FORMAT_VERSION." >&2
  echo "Please install the required version to proceed or use format-code-docker.sh." >&2
  exit 1
fi

if [ $# -eq 0 ]
then
    # No arguments: format all
    SOURCE_DIRS="libs/ solver/ tools/ config/ tests/ packaging/ api/ io/ optimisation/ expressions/ study/ modeler/"
    SOURCE_FILES=$(find $SOURCE_DIRS -regextype egrep -regex ".*/*\.(c|cxx|cpp|cc|h|hxx|hpp)$" ! -path '*/additionalConstraintRhsExpression/*' ! -path '*/scenarioBuilderExpression/*' ! -path '*/antlr-interface/*' ! -path '*/reservesParsing/*')
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

if ! [ -x "$(command -v clang-format)" ]; then
    echo 'Warning: clang-format is not installed. Skipping' >&2
else
    echo "$SOURCE_FILES" | xargs clang-format  -style=file:../.clang-format -i --verbose
fi
