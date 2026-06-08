#!/bin/bash

set -euo pipefail

# Configuration
CLANG_FORMAT_IMAGE="antares/clang-format:18"
REBUILD_IMAGE=false
REQUIRED_CLANG_FORMAT_VERSION="18.1.3"

# Resolve locations
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DOCKERFILE="$PROJECT_ROOT/docker/clang-format.Dockerfile"

# Globals
FILES=()
USE_DOCKER=true
ARGS=()

# ---------------------- helpers ----------------------
usage() {
  cat <<'USAGE'
Usage: bash src/format-code-docker.sh [--rebuild-image|-r] [files...]

Options:
  --rebuild-image, -r   Force rebuild of the clang-format Docker image before formatting
  --help, -h            Show this help message

If no files are provided, the script finds sources under a set of src/ subdirectories.
USAGE
}

parse_args() {
  ARGS=()
  while [ "$#" -gt 0 ]; do
    case "$1" in
      --rebuild-image|-r)
        REBUILD_IMAGE=true
        shift
        ;;
      --help|-h)
        usage
        exit 0
        ;;
      --)
        shift
        while [ "$#" -gt 0 ]; do ARGS+=("$1"); shift; done
        ;;
      -* )
        echo "Unknown option: $1" >&2; usage; exit 1
        ;;
      *)
        ARGS+=("$1")
        shift
        ;;
    esac
  done
}

# Gather files to format into FILES[]
gather_files() {
  FILES=()
  if [ ${#ARGS[@]} -eq 0 ]; then
    SOURCE_DIRS=(libs/ solver/ tools/ config/ tests/ packaging/ api/ io/ optimisation/ expressions/ study/ modeler/)
    # Run find from SCRIPT_DIR so results are relative to src
    while IFS= read -r f; do
      [ -n "$f" ] && FILES+=("$f")
    done < <(cd "$SCRIPT_DIR" && find "${SOURCE_DIRS[@]}" -regextype egrep -regex ".*\\.(c|cxx|cpp|cc|h|hxx|hpp)$" \
                 ! -path '*/additionalConstraintRhsExpression/*' ! -path '*/scenarioBuilderExpression/*' ! -path '*/antlr-interface/*' ! -path '*/reservesParsing/*' 2>/dev/null)
  else
    for a in "${ARGS[@]}"; do
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

  if [ ${#FILES[@]} -eq 0 ]; then
    echo "No files to format."
    exit 0
  fi
}

# Normalize CRLF -> LF when dos2unix is available
normalize_line_endings() {
  if command -v dos2unix >/dev/null 2>&1; then
    # Silence dos2unix output (stdout and stderr). Keep || true so the script doesn't fail if dos2unix
    # can't process a file (e.g. binary files) or xargs finds nothing.
    printf '%s\n' "${FILES[@]}" | xargs -r -I{} dos2unix "{}" >/dev/null 2>&1 || true
  fi
}

# Check local clang-format version
check_local_clang() {
  USE_DOCKER=true
  if command -v clang-format >/dev/null 2>&1; then
    CLANG_FORMAT_VERSION=$(clang-format --version 2>&1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || true)
    if [ "$CLANG_FORMAT_VERSION" = "$REQUIRED_CLANG_FORMAT_VERSION" ]; then
      echo "✓ clang-format $REQUIRED_CLANG_FORMAT_VERSION found locally"
      USE_DOCKER=false
    else
      echo "✗ Found clang-format version ${CLANG_FORMAT_VERSION:-unknown} (need $REQUIRED_CLANG_FORMAT_VERSION). Will try Docker image $CLANG_FORMAT_IMAGE"
    fi
  else
    echo "✗ clang-format not found locally. Will try Docker image $CLANG_FORMAT_IMAGE"
  fi
}

# Ensure docker is accessible; if not, fallback to local clang-format when available
ensure_docker_access() {
  if ! command -v docker >/dev/null 2>&1; then
    # docker not installed -> try local clang-format
    if command -v clang-format >/dev/null 2>&1; then
      CLANG_FORMAT_VERSION_LOCAL=$(clang-format --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || true)
      echo "Warning: docker not installed; using local clang-format (version ${CLANG_FORMAT_VERSION_LOCAL:-unknown})." >&2
      USE_DOCKER=false
      return 0
    fi
    echo "Error: docker not installed and no local clang-format found." >&2
    return 1
  fi

  if ! docker info >/dev/null 2>&1; then
    # docker daemon inaccessible -> try local clang-format
    if command -v clang-format >/dev/null 2>&1; then
      CLANG_FORMAT_VERSION_LOCAL=$(clang-format --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || true)
      echo "Warning: docker daemon not accessible; falling back to local clang-format (version ${CLANG_FORMAT_VERSION_LOCAL:-unknown})." >&2
      echo "Note: project requires clang-format $REQUIRED_CLANG_FORMAT_VERSION; consider installing that version or enabling Docker." >&2
      USE_DOCKER=false
      return 0
    fi

    cat >&2 <<'MSG'
Error: docker appears installed but cannot access the docker daemon (permission denied or daemon not running).
Possible fixes:
  - If you're on Linux/WSL and Docker requires sudo, add your user to the docker group and re-login:
      sudo usermod -aG docker $USER
      newgrp docker   # or log out and back in
  - Or run this script with sudo: sudo bash src/format-code.sh [files]
  - Ensure the Docker daemon is running: sudo systemctl start docker (or start your Docker Desktop on Windows)
  - On WSL2, ensure Docker Desktop integration for your distro is enabled and the daemon running.
After fixing, re-run this script.
MSG
    return 1
  fi

  return 0
}

# Build image if needed
select_and_build_image() {
  if [ ! -f "$DOCKERFILE" ]; then
    echo "Error: Dockerfile not found at $DOCKERFILE" >&2
    return 1
  fi

  if [ "$REBUILD_IMAGE" = true ]; then
    if docker image inspect "$CLANG_FORMAT_IMAGE" >/dev/null 2>&1; then
      echo "Forcing rebuild: removing existing image $CLANG_FORMAT_IMAGE"
      docker rmi "$CLANG_FORMAT_IMAGE" || true
    fi
  fi

  if ! docker image inspect "$CLANG_FORMAT_IMAGE" >/dev/null 2>&1; then
    echo "Building image $CLANG_FORMAT_IMAGE using $DOCKERFILE..."
    docker build -t "$CLANG_FORMAT_IMAGE" -f "$DOCKERFILE" "$PROJECT_ROOT"
  else
    echo "Docker image $CLANG_FORMAT_IMAGE already present"
  fi
}

# Verify clang-format inside image
verify_image_clang_version() {
  echo "Verifying clang-format version inside Docker image $CLANG_FORMAT_IMAGE..."
  IMAGE_CLANG_VER=$(docker run --rm "$CLANG_FORMAT_IMAGE" clang-format --version 2>/dev/null | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' || true)
  if [ -z "$IMAGE_CLANG_VER" ]; then
    echo "Error: could not determine clang-format version inside Docker image $CLANG_FORMAT_IMAGE." >&2
    return 1
  fi

  if [ "$IMAGE_CLANG_VER" != "$REQUIRED_CLANG_FORMAT_VERSION" ]; then
    echo "Error: clang-format inside Docker image $CLANG_FORMAT_IMAGE is version $IMAGE_CLANG_VER but required is $REQUIRED_CLANG_FORMAT_VERSION." >&2
    echo "Please rebuild the image to include the required version or install it locally." >&2
    return 1
  fi

  echo "✓ clang-format inside Docker image is $IMAGE_CLANG_VER"
  return 0
}

# Formatting functions
format_with_docker() {
  if [ -f "$SCRIPT_DIR/.clang-format" ]; then
    STYLE_DOCKER="-style=file:/workspace/.clang-format"
  else
    STYLE_DOCKER=""
  fi

  TMP_LIST=$(mktemp "$SCRIPT_DIR/format-list.XXXXXX")
  trap 'rm -f "$TMP_LIST"' EXIT
  printf '%s\n' "${FILES[@]}" > "$TMP_LIST"
  INSIDE_LIST="/workspace/src/$(basename "$TMP_LIST")"
  DOCKER_USER="$(id -u):$(id -g)"

  echo "Formatting ${#FILES[@]} files using Docker image $CLANG_FORMAT_IMAGE..."
  if [ -n "$STYLE_DOCKER" ]; then
    docker run --rm --user "$DOCKER_USER" -v "$PROJECT_ROOT:/workspace" -w /workspace/src "$CLANG_FORMAT_IMAGE" bash -c "set -euo pipefail; while IFS= read -r file || [ -n \"\$file\" ]; do if [ -n \"\$file\" ]; then echo \"Formatting: \$file\"; clang-format $STYLE_DOCKER -i --verbose \"\$file\"; fi; done < $INSIDE_LIST"
  else
    docker run --rm --user "$DOCKER_USER" -v "$PROJECT_ROOT:/workspace" -w /workspace/src "$CLANG_FORMAT_IMAGE" bash -c "set -euo pipefail; while IFS= read -r file || [ -n \"\$file\" ]; do if [ -n \"\$file\" ]; then echo \"Formatting: \$file\"; clang-format -i --verbose \"\$file\"; fi; done < $INSIDE_LIST"
  fi
}

format_with_local() {
  echo "Formatting ${#FILES[@]} files using local clang-format..."
  if [ -f "$SCRIPT_DIR/.clang-format" ]; then
    STYLE_LOCAL="-style=file:$SCRIPT_DIR/.clang-format"
  else
    STYLE_LOCAL=""
  fi
  for f in "${FILES[@]}"; do
    echo "Formatting: $f"
    if [ -n "$STYLE_LOCAL" ]; then
      clang-format $STYLE_LOCAL -i --verbose "$f"
    else
      clang-format -i --verbose "$f"
    fi
  done
}

# ---------------------- main ----------------------
main() {
  parse_args "$@"
  gather_files
  normalize_line_endings
  check_local_clang

  if [ "$USE_DOCKER" = true ]; then
    if ! ensure_docker_access; then
      echo "Docker unavailable and no local clang-format: aborting." >&2
      exit 1
    fi

    select_and_build_image || exit 1
    verify_image_clang_version || exit 1
    format_with_docker
    exit 0
  fi

  # fallback: use local clang-format
  format_with_local
}

main "$@"
