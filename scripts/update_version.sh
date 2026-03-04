#!/usr/bin/env bash
# Script to bump project version in CMakeLists and create a git commit + tag
# Usage: scripts/update_version.sh [options] <new-version>
# Options: --no-push, --force, --dry-run, --message, --tag, --sign, --run-tests

set -euo pipefail
IFS=$'\n\t'

PROGNAME="$(basename "$0")"
REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null || echo '.')"

# Defaults
NO_PUSH=0
FORCE=0
DRY_RUN=0
SIGN_TAG=0
RUN_TESTS=0
COMMIT_MSG=""
TAG_NAME=""
OVERWRITE_TAG=0

usage() {
  cat <<EOF
Usage: $PROGNAME [options] <new-version>

Options:
  -n, --no-push        Do not push commit or tag to remote
  -f, --force          Allow running with dirty working tree or detached HEAD
  -d, --dry-run        Show planned changes and exit without modifying files
  -m, --message <msg>  Commit message (default: "Bump version to <new-version>")
  -t, --tag <tag>      Tag name to create (default: v<new-version>)
      --sign           Sign the tag with GPG
      --run-tests      Run a build+test after updating (abort on failure)
      --overwrite-tag  Overwrite existing tag if present (implies --force)
  -h, --help           Show this help and exit
EOF
}

# Minimal argument parsing
if [ $# -eq 0 ]; then
  usage
  exit 2
fi

# Parse
POSITIONAL=()
while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
    -n|--no-push)
      NO_PUSH=1; shift;;
    -f|--force)
      FORCE=1; shift;;
    -d|--dry-run)
      DRY_RUN=1; shift;;
    -m|--message)
      COMMIT_MSG="$2"; shift 2;;
    -t|--tag)
      TAG_NAME="$2"; shift 2;;
    --sign)
      SIGN_TAG=1; shift;;
    --run-tests)
      RUN_TESTS=1; shift;;
    --overwrite-tag)
      OVERWRITE_TAG=1; FORCE=1; shift;;
    -h|--help)
      usage; exit 0;;
    --)
      shift; break;;
    -*)
      echo "Unknown option: $1" >&2; usage; exit 2;;
    *)
      POSITIONAL+=("$1"); shift;;
  esac
done
set -- "${POSITIONAL[@]}"

if [ ${#POSITIONAL[@]} -lt 1 ]; then
  echo "Error: missing <new-version>" >&2; usage; exit 2
fi
NEW_VERSION="${POSITIONAL[0]}"

# Validate version: simple regex like X.Y.Z or X.Y.Z-extra
if ! [[ "$NEW_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+([-.].*)?$ ]]; then
  echo "Invalid version format: $NEW_VERSION. Expected X.Y.Z" >&2; exit 2
fi

if [ -z "$COMMIT_MSG" ]; then
  COMMIT_MSG="Bump version to $NEW_VERSION"
fi
if [ -z "$TAG_NAME" ]; then
  TAG_NAME="v$NEW_VERSION"
fi

# Ensure we're in a git repo
if ! git rev-parse --git-dir >/dev/null 2>&1; then
  echo "Not a git repository (or any of the parent directories)." >&2
  exit 2
fi

# Gather git info
BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "HEAD")
if [ "$BRANCH" = "HEAD" ] && [ $FORCE -ne 1 ]; then
  echo "Detached HEAD. Use --force to override." >&2; exit 3
fi

# Check for dirty tree
STATUS=$(git status --porcelain)
if [ -n "$STATUS" ] && [ $FORCE -ne 1 ]; then
  echo "Working tree is not clean. Please commit or stash changes, or use --force to proceed." >&2
  echo "Changed files:" >&2
  echo "$STATUS" >&2
  exit 4
fi

# Check tag existence
if git rev-parse --verify --quiet "refs/tags/$TAG_NAME" >/dev/null; then
  if [ $OVERWRITE_TAG -ne 1 ]; then
    echo "Tag $TAG_NAME already exists. Use --overwrite-tag to replace it." >&2
    exit 5
  else
    echo "Warning: tag $TAG_NAME exists and will be overwritten (force mode)."
  fi
fi

# Parse version components
IFS='.' read -r VER_HI VER_LO VER_REV <<< "$(echo "$NEW_VERSION" | cut -d'-' -f1)"

# Files to update: primary target
CMAKE_FILE="$REPO_ROOT/src/CMakeLists.txt"
SONAR_FILE="$REPO_ROOT/sonar-project.properties"
SONAR_PRESENT=0
if [ -f "$SONAR_FILE" ]; then
  SONAR_PRESENT=1
fi
if [ ! -f "$CMAKE_FILE" ]; then
  echo "Expected file not found: $CMAKE_FILE" >&2
  exit 2
fi

# Prepare in-place update function
update_cmake_version() {
  local file="$1"
  local hi="$2"; local lo="$3"; local rev="$4"; local year="$5"
  local tmp
  tmp=$(mktemp)
  awk -v hi="$hi" -v lo="$lo" -v rev="$rev" -v year="$year" '
    /^set\(ANTARES_VERSION_HI[[:space:]]+[0-9]+\)/ { print "set(ANTARES_VERSION_HI " hi ")"; next }
    /^set\(ANTARES_VERSION_LO[[:space:]]+[0-9]+\)/ { print "set(ANTARES_VERSION_LO " lo ")"; next }
    /^set\(ANTARES_VERSION_REVISION[[:space:]]+[0-9]+\)/ { print "set(ANTARES_VERSION_REVISION " rev ")"; next }
    /^set\(ANTARES_VERSION_YEAR[[:space:]]+[0-9]+\)/ { if (year != "") print "set(ANTARES_VERSION_YEAR " year ")"; else print; next }
    { print }
  ' "$file" > "$tmp"
  mv "$tmp" "$file"
}

# Dry-run: show planned replacement and exit
if [ $DRY_RUN -eq 1 ]; then
  echo "DRY RUN: would update $CMAKE_FILE with:"
  echo "  ANTARES_VERSION_HI = $VER_HI"
  echo "  ANTARES_VERSION_LO = $VER_LO"
  echo "  ANTARES_VERSION_REVISION = $VER_REV"
  if grep -q "ANTARES_VERSION_YEAR" "$CMAKE_FILE"; then
    echo "  ANTARES_VERSION_YEAR = <current year> (will be updated automatically)"
  fi
  if [ $SONAR_PRESENT -eq 1 ]; then
    if grep -q '^sonar.projectVersion=' "$SONAR_FILE"; then
      CUR_SONAR_VER=$(grep '^sonar.projectVersion=' "$SONAR_FILE" | cut -d'=' -f2-)
      echo "DRY RUN: would update $SONAR_FILE: sonar.projectVersion = $CUR_SONAR_VER -> $NEW_VERSION"
    else
      echo "DRY RUN: would add to $SONAR_FILE: sonar.projectVersion=$NEW_VERSION"
    fi
  fi
  echo
  echo "Planned git operations:"
  if [ $SONAR_PRESENT -eq 1 ]; then
    echo "  git add $CMAKE_FILE $SONAR_FILE"
  else
    echo "  git add $CMAKE_FILE"
  fi
   echo "  git commit -m '$COMMIT_MSG'"
   echo "  git tag -a $TAG_NAME -m 'Release $TAG_NAME'"
   if [ $NO_PUSH -eq 0 ]; then
     echo "  git push origin $BRANCH"
     echo "  git push origin $TAG_NAME"
   else
     echo "  (no push due to --no-push)"
   fi
   exit 0
 fi

# Real run: update the CMake file(s)
CURRENT_YEAR="$(date +%Y)"
YEAR_TO_SET="$CURRENT_YEAR"

# Make a backup copy first
BACKUP_DIR=$(mktemp -d)
cp "$CMAKE_FILE" "$BACKUP_DIR/$(basename "$CMAKE_FILE").bak"
if [ $SONAR_PRESENT -eq 1 ]; then
  cp "$SONAR_FILE" "$BACKUP_DIR/$(basename "$SONAR_FILE").bak"
fi

 update_cmake_version "$CMAKE_FILE" "$VER_HI" "$VER_LO" "$VER_REV" "$YEAR_TO_SET"

# Update sonar-project.properties if present: replace sonar.projectVersion or append it
if [ $SONAR_PRESENT -eq 1 ]; then
  tmp=$(mktemp)
  awk -v ver="$NEW_VERSION" 'BEGIN{found=0} /^sonar.projectVersion[[:space:]]*=/ { print "sonar.projectVersion=" ver; found=1; next } { print } END { if (!found) print "sonar.projectVersion=" ver }' "$SONAR_FILE" > "$tmp"
  mv "$tmp" "$SONAR_FILE"
fi

 # Check that the file actually changed
-if git diff --no-ext-diff --quiet -- "$CMAKE_FILE"; then
-  echo "No changes detected in $CMAKE_FILE after update. Aborting." >&2
-  exit 6
+CHANGED=0
+if ! git diff --no-ext-diff --quiet -- "$CMAKE_FILE"; then
+  CHANGED=1
+fi
+if [ $SONAR_PRESENT -eq 1 ]; then
+  if ! git diff --no-ext-diff --quiet -- "$SONAR_FILE"; then
+    CHANGED=1
+  fi
+fi
+if [ $CHANGED -eq 0 ]; then
+  echo "No changes detected in updated files after update. Aborting." >&2
+  exit 6
 fi

 # Stage only the modified file
git add "$CMAKE_FILE"
if [ $SONAR_PRESENT -eq 1 ]; then
  git add "$SONAR_FILE"
fi

# Commit
 if ! git commit -m "$COMMIT_MSG"; then
   echo "git commit failed. Restoring backup." >&2
   mv "$BACKUP_DIR/$(basename "$CMAKE_FILE").bak" "$CMAKE_FILE"
+  if [ $SONAR_PRESENT -eq 1 ]; then
+    mv "$BACKUP_DIR/$(basename "$SONAR_FILE").bak" "$SONAR_FILE"
+  fi
   git reset -- "$CMAKE_FILE" || true
   exit 7
 fi

 COMMIT_HASH=$(git rev-parse --short HEAD)

# Create tag
if [ $SIGN_TAG -eq 1 ]; then
  git tag -s "$TAG_NAME" -m "Release $TAG_NAME"
else
  git tag -a "$TAG_NAME" -m "Release $TAG_NAME"
fi

# Push if requested
if [ $NO_PUSH -eq 0 ]; then
  echo "Pushing commit and tag to origin/$BRANCH..."
  git push origin "$BRANCH"
  git push origin "$TAG_NAME"
else
  echo "Skipping push ( --no-push ). Commit: $COMMIT_HASH, Tag: $TAG_NAME"
fi

# Optional: run tests/build
if [ $RUN_TESTS -eq 1 ]; then
  echo "Running build and tests (this may take a while)..."
  set +e
  mkdir -p "$REPO_ROOT/build"
  pushd "$REPO_ROOT/build" >/dev/null
  cmake .. && cmake --build . -- -j"$(nproc)"
  CT_EXIT=0
  if command -v ctest >/dev/null 2>&1; then
    ctest -j2 || CT_EXIT=$?
  fi
  popd >/dev/null
  set -e
  if [ $CT_EXIT -ne 0 ]; then
    echo "Build or tests failed (exit $CT_EXIT). Please inspect build logs." >&2
    exit 8
  fi
fi

echo "Success: committed $COMMIT_HASH and created tag $TAG_NAME"

# cleanup
rm -rf "$BACKUP_DIR"
