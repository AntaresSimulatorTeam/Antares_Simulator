#!/bin/bash

# Docker image to use for formatting
DOCKER_IMAGE="ubuntu:24.04"
CONTAINER_NAME="clang-format-runner"
USE_DOCKER=false
# Name/tag for the prebuilt image that contains clang-format
CLANG_FORMAT_IMAGE="antares/clang-format:18"
# Force rebuild the image if --rebuild-image is passed or env var is set
REBUILD_IMAGE=false

# Ensure we're in the src directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Parse optional flag for rebuilding the image
if [ "$#" -gt 0 ]; then
    if [ "$1" = "--rebuild-image" ] || [ "$1" = "-r" ]; then
        REBUILD_IMAGE=true
        shift
    fi
fi

if [ $# -eq 0 ]
then
    # No arguments: format all
    SOURCE_DIRS="libs/ solver/ tools/ config/ tests/ packaging/ api/ io/ optimisation/ expressions/ study/ modeler/"
    SOURCE_FILES=$(find $SOURCE_DIRS -regextype egrep -regex ".*/*\.(c|cxx|cpp|cc|h|hxx|hpp)$" ! -path '*/additionalConstraintRhsExpression/*' ! -path '*/scenarioBuilderExpression/*' ! -path '*/antlr-interface/*')
else
    # Format files provided as arguments
    SOURCE_FILES="$*"
fi

# Remove ^M, etc.
if ! [ -x "$(command -v dos2unix)" ]; then
    echo 'Warning: dos2unix is not installed. Skipping' >&2
else
    echo "$SOURCE_FILES" | xargs dos2unix
fi

# Check if clang-format is available locally
echo "Checking for clang-format locally..."
if command -v clang-format &> /dev/null; then
    CLANG_FORMAT_VERSION=$(clang-format --version 2>&1 | grep -oP '\\d+\\.\\d+\\.\\d+')
    if [ "$CLANG_FORMAT_VERSION" = "18.1.3" ]; then
        echo "✓ clang-format 18.1.3 found locally"
        USE_DOCKER=false
    else
        echo "✗ Found clang-format version $CLANG_FORMAT_VERSION (need 18.1.3). Using Docker..."
        USE_DOCKER=true
    fi
else
    echo "✗ clang-format not found locally. Using Docker..."
    USE_DOCKER=true
fi

if [ "$USE_DOCKER" = true ]; then
    # Check if Docker is available
    if ! [ -x "$(command -v docker)" ]; then
        echo 'Error: docker is not installed. Please install Docker or clang-format 18.1.3 locally.' >&2
        exit 1
    fi

    # If requested, remove the existing image to force rebuild
    if [ "$REBUILD_IMAGE" = true ]; then
        if docker image inspect "$CLANG_FORMAT_IMAGE" > /dev/null 2>&1; then
            echo "Forcing rebuild: removing existing image $CLANG_FORMAT_IMAGE"
            docker rmi "$CLANG_FORMAT_IMAGE" || true
        fi
    fi

    # Build the image only if it doesn't exist
    if ! docker image inspect "$CLANG_FORMAT_IMAGE" > /dev/null 2>&1; then
        echo "Building Docker image $CLANG_FORMAT_IMAGE with clang-format 18..."
        docker build -t "$CLANG_FORMAT_IMAGE" - <<'DOCKERFILE'
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y wget gnupg software-properties-common ca-certificates apt-transport-https \
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && add-apt-repository 'deb http://apt.llvm.org/noble/ llvm-toolchain-noble-18 main' \
    && apt-get update && apt-get install -y clang-format-18 \
    && ln -sf /usr/bin/clang-format-18 /usr/bin/clang-format \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
DOCKERFILE
        if [ $? -ne 0 ]; then
            echo "Error: docker build failed" >&2
            exit 1
        fi
    else
        echo "Docker image $CLANG_FORMAT_IMAGE already present."
    fi

    if [ -n "$SOURCE_FILES" ]; then
        echo "Formatting files (using Docker image $CLANG_FORMAT_IMAGE)..."
        # Run a disposable container and feed the list of files to a small shell snippet
        echo "$SOURCE_FILES" | docker run -i --rm -v "$PROJECT_ROOT:/workspace" -w /workspace/src "$CLANG_FORMAT_IMAGE" bash -s -- <<'EOF'
while IFS= read -r file; do
    if [ -n "$file" ]; then
        echo "Formatting: $file"
        clang-format -style=file:/workspace/.clang-format -i --verbose "$file"
    fi
done
EOF
    else
        echo "No files to format."
    fi
else
    # Use local clang-format
    if [ -n "$SOURCE_FILES" ]; then
        echo "Formatting files (using local clang-format)..."
        echo "$SOURCE_FILES" | xargs clang-format -style=file:"$SCRIPT_DIR/.clang-format" -i --verbose
    else
        echo "No files to format."
    fi
fi

echo "Formatting complete!"
