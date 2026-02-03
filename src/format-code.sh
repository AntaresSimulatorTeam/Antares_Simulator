#!/bin/bash

# Docker image to use for formatting
DOCKER_IMAGE="ubuntu:24.04"
CONTAINER_NAME="clang-format-runner"
USE_DOCKER=false

# Ensure we're in the src directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

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
    CLANG_FORMAT_VERSION=$(clang-format --version 2>&1 | grep -oP '\d+\.\d+\.\d+')
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

    # Pull the Ubuntu image if not present
    echo "Checking for Docker image $DOCKER_IMAGE..."
    if ! docker image inspect "$DOCKER_IMAGE" > /dev/null 2>&1; then
        echo "Pulling $DOCKER_IMAGE..."
        docker pull "$DOCKER_IMAGE"
    fi

    # Clean up any existing container with the same name
    echo "Cleaning up any existing container..."
    docker stop "$CONTAINER_NAME" > /dev/null 2>&1
    docker rm "$CONTAINER_NAME" > /dev/null 2>&1

    # Create a temporary container with clang-format installed
    echo "Setting up container with clang-format..."
    docker run -d \
        --name "$CONTAINER_NAME" \
        -v "$PROJECT_ROOT:/workspace" \
        -w /workspace/src \
        "$DOCKER_IMAGE" \
        sleep infinity

    # Check if container was created successfully
    if ! docker inspect "$CONTAINER_NAME" > /dev/null 2>&1; then
        echo "Error: Failed to create Docker container" >&2
        exit 1
    fi

    # Wait a moment for the container to be ready
    sleep 2

    # Install clang-format 18 in the container via LLVM PPA
    echo "Installing clang-format 18 in container..."
    docker exec "$CONTAINER_NAME" bash -c "apt-get update && apt-get install -y wget software-properties-common && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && add-apt-repository 'deb http://apt.llvm.org/noble/ llvm-toolchain-noble-18 main' && apt-get update && apt-get install -y clang-format-18 && ln -sf /usr/bin/clang-format-18 /usr/bin/clang-format"

    if [ -n "$SOURCE_FILES" ]; then
        echo "Formatting files (using Docker)..."
        echo "$SOURCE_FILES" | xargs -I {} docker exec "$CONTAINER_NAME" \
            clang-format -style=file:/workspace/.clang-format -i --verbose {}
    else
        echo "No files to format."
    fi

    # Clean up: stop and remove the container
    echo "Cleaning up container..."
    docker stop "$CONTAINER_NAME" > /dev/null 2>&1
    docker rm "$CONTAINER_NAME" > /dev/null 2>&1
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

