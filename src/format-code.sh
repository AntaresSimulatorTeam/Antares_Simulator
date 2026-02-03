#!/bin/bash

set -u

# Configure
CLANG_FORMAT_IMAGE="antares/clang-format:18"
REBUILD_IMAGE=false

# Resolve script dir and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
# Use a single Dockerfile located in the project's docker/ directory
DOCKERFILE="$PROJECT_ROOT/docker/clang-format.Dockerfile"

# Move to script dir so relative paths and find behave consistently
cd "$SCRIPT_DIR" || exit 1

# Parse flags
if [ "$#" -gt 0 ]; then
  if [ "$1" = "--rebuild-image" ] || [ "$1" = "-r" ]; then
    REBUILD_IMAGE=true
    shift
  fi
fi

# Build file list
FILES=()
if [ "$#" -eq 0 ]; then
  SOURCE_DIRS=(libs/ solver/ tools/ config/ tests/ packaging/ api/ io/ optimisation/ expressions/ study/ modeler/)
  while IFS= read -r f; do
    [ -n "$f" ] && FILES+=("$f")
  done < <(find "${SOURCE_DIRS[@]}" -regextype egrep -regex ".*/*\.(c|cxx|cpp|cc|h|hxx|hpp)$" ! -path '*/additionalConstraintRhsExpression/*' ! -path '*/scenarioBuilderExpression/*' ! -path '*/antlr-interface/*' 2>/dev/null)
else
  for a in "$@"; do
    # normalize to path relative to src
    if [[ "$a" = /* ]] && [[ "$a" == "$PROJECT_ROOT"/* ]]; then
      rel="${a#$PROJECT_ROOT/}"
    else
      rel="$a"
    fi
    if [[ "$rel" == src/* ]]; then
      rel="${rel#src/}"
    fi
    FILES+=("$rel")
  done
fi

# If no files, nothing to do
if [ ${#FILES[@]} -eq 0 ]; then
  echo "No files to format."
  exit 0
fi

# Normalize line endings if possible
if command -v dos2unix >/dev/null 2>&1; then
  printf '%s\n' "${FILES[@]}" | xargs -r dos2unix
else
  echo "Warning: dos2unix not found; skipping line-ending normalization" >&2
fi

# Check local clang-format
USE_DOCKER=true
if command -v clang-format >/dev/null 2>&1; then
  CLANG_FORMAT_VERSION=$(clang-format --version 2>&1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+') || CLANG_FORMAT_VERSION=""
  if [ "$CLANG_FORMAT_VERSION" = "18.1.3" ]; then
    echo "✓ clang-format 18.1.3 found locally"
    USE_DOCKER=false
  else
    echo "✗ Found clang-format version ${CLANG_FORMAT_VERSION:-unknown} (need 18.1.3). Will use Docker image $CLANG_FORMAT_IMAGE"
  fi
else
  echo "✗ clang-format not found locally. Will use Docker image $CLANG_FORMAT_IMAGE"
fi

if [ "$USE_DOCKER" = true ]; then
  if ! command -v docker >/dev/null 2>&1; then
    echo "Error: docker not available and local clang-format is unsuitable" >&2
    exit 1
  fi

  # Verify access to docker daemon to avoid permission denied during build
  if ! docker info >/dev/null 2>&1; then
    cat >&2 <<'MSG'
Error: docker appears installed but cannot access the docker daemon (permission denied or daemon not running).
Possible fixes:
  - If you're on Linux/WSL and Docker requires sudo, add your user to the docker group and re-login:
      sudo usermod -aG docker $USER
      newgrp docker   # or log out and back in
  - Or run this script with sudo: sudo bash src/format-code.sh [files]
  - Ensure the Docker daemon is running: sudo systemctl start docker (or start your Docker Desktop on Windows)
  - On WSL2, ensure Docker Desktop integration for your distro is enabled and the daemon running.
After fixing, re-run this script. The script will now exit to avoid a partially-built image.
MSG
    exit 1
  fi

  # Select Dockerfile
  if [ -f "$DOCKERFILE" ]; then
    : # dockerfile found, DOCKERFILE already set
  else
    echo "Error: Dockerfile not found at $DOCKERFILE" >&2
    exit 1
  fi

  # Rebuild image if requested
  if [ "$REBUILD_IMAGE" = true ]; then
    if docker image inspect "$CLANG_FORMAT_IMAGE" >/dev/null 2>&1; then
      echo "Forcing rebuild: removing existing image $CLANG_FORMAT_IMAGE"
      docker rmi "$CLANG_FORMAT_IMAGE" || true
    fi
  fi

  # Build image if missing
  if ! docker image inspect "$CLANG_FORMAT_IMAGE" >/dev/null 2>&1; then
    echo "Building image $CLANG_FORMAT_IMAGE using $DOCKERFILE..."
    docker build -t "$CLANG_FORMAT_IMAGE" -f "$DOCKERFILE" "$SCRIPT_DIR"
  else
    echo "Docker image $CLANG_FORMAT_IMAGE already present"
  fi

  # --- NEW: Verify clang-format version inside the Docker image ---
  if command -v docker >/dev/null 2>&1; then
    echo "Verifying clang-format version inside Docker image $CLANG_FORMAT_IMAGE..."
    IMAGE_CLANG_VER=$(docker run --rm "$CLANG_FORMAT_IMAGE" clang-format --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || true)
    if [ -z "$IMAGE_CLANG_VER" ]; then
      # Could not determine version: show hint and exit
      cat >&2 <<'MSG'
Error: could not determine clang-format version inside Docker image $CLANG_FORMAT_IMAGE.
This may be due to inability to run containers (permissions) or the image missing clang-format.
Possible actions:
  - Ensure you can run containers (test: docker run --rm $CLANG_FORMAT_IMAGE clang-format --version)
  - Rebuild the image locally with the Dockerfile: bash src/format-code.sh --rebuild-image
MSG
      exit 1
    fi

    if [ "$IMAGE_CLANG_VER" != "18.1.3" ]; then
      cat >&2 <<MSG
Error: clang-format inside Docker image $CLANG_FORMAT_IMAGE is version $IMAGE_CLANG_VER but the project requires 18.1.3.
Options:
  1) Rebuild the image so it contains the required version:
       bash src/format-code.sh --rebuild-image
  2) Install clang-format 18.1.3 locally and run the script without Docker.
After fixing, re-run this script.
MSG
      exit 1
    else
      echo "✓ clang-format inside Docker image is $IMAGE_CLANG_VER"
    fi
  fi
  # --- END new verification ---

  # Write file list to a temporary file inside src so it is visible in the container
  TMP_LIST=$(mktemp "$SCRIPT_DIR/format-list.XXXXXX")
  trap 'rm -f "$TMP_LIST"' EXIT
  printf '%s\n' "${FILES[@]}" > "$TMP_LIST"
  INSIDE_LIST="/workspace/src/$(basename "$TMP_LIST")"

  echo "Formatting ${#FILES[@]} files using Docker image $CLANG_FORMAT_IMAGE..."
  docker run --rm -v "$PROJECT_ROOT:/workspace" -w /workspace/src "$CLANG_FORMAT_IMAGE" bash -c "while IFS= read -r file || [ -n \"\$file\" ]; do if [ -n \"\$file\" ]; then echo \"Formatting: \$file\"; clang-format -style=file:/workspace/.clang-format -i --verbose \"\$file\"; fi; done < $INSIDE_LIST"

else
  echo "Formatting ${#FILES[@]} files using local clang-format..."
  printf '%s\n' "${FILES[@]}" | xargs -r clang-format -style=file:"$SCRIPT_DIR/.clang-format" -i --verbose
fi

echo "Formatting complete!"
