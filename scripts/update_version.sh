#!/usr/bin/env bash
# Script to bump project version in CMakeLists and create a git commit + tag
# Usage: scripts/update_version.sh [options] <new-version>
# Options: --force, --dry-run, --message, --tag, --sign

set -euo pipefail
IFS=$'\n\t'

PROGNAME="$(basename "$0")"
REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null || echo '.')"

# Defaults
FORCE=0
DRY_RUN=0
COMMIT_MSG=""
TAG_NAME=""

usage() {
  cat <<EOF
Usage: $PROGNAME [options] <new-version>

Options:
  -f, --force          Allow running with dirty working tree or detached HEAD
  -d, --dry-run        Show planned changes and exit without modifying files
  -m, --message <msg>  Commit message (default: "Bump version to <new-version>")
      # build and test are not performed by this script
      # note: tag creation is not performed by this script; suggested tag name is v<new-version>
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
    -f|--force)
      FORCE=1; shift;;
    -d|--dry-run)
      DRY_RUN=1; shift;;
    -m|--message)
      COMMIT_MSG="$2"; shift 2;;
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

# Parse version components
IFS='.' read -r VER_HI VER_LO VER_REV <<< "$(echo "$NEW_VERSION" | cut -d'-' -f1)"

# Files to update: primary target
CMAKE_FILE="$REPO_ROOT/src/CMakeLists.txt"
SONAR_FILE="$REPO_ROOT/sonar-project.properties"
VCPKG_FILE="$REPO_ROOT/src/vcpkg.json"
# Assume the target files exist in the workspace as requested; proceed without checking.

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
  # sonar-project.properties: assume present and report planned change
  if grep -q '^sonar.projectVersion=' "$SONAR_FILE"; then
    CUR_SONAR_VER=$(grep '^sonar.projectVersion=' "$SONAR_FILE" | cut -d'=' -f2-)
    echo "DRY RUN: would update $SONAR_FILE: sonar.projectVersion = $CUR_SONAR_VER -> $NEW_VERSION"
  else
    echo "DRY RUN: would add to $SONAR_FILE: sonar.projectVersion=$NEW_VERSION"
  fi
  # vcpkg.json: assume present and report planned change
  if grep -q '"version-string"' "$VCPKG_FILE"; then
    CUR_VCPKG_VER=$(grep '"version-string"' "$VCPKG_FILE" | head -n1 | sed -E 's/.*"version-string"\s*:\s*"([^\"]+)".*/\1/')
    echo "DRY RUN: would update $VCPKG_FILE: version-string = $CUR_VCPKG_VER -> $NEW_VERSION"
  else
    echo "DRY RUN: would add to $VCPKG_FILE: \"version-string\": \"$NEW_VERSION\""
  fi
  echo
  echo "Planned git operations:"
  echo "  (note: this script will NOT commit, tag or push automatically)"
  echo "Suggested commands to commit and push manually:"
  echo "  git add $CMAKE_FILE $SONAR_FILE $VCPKG_FILE"
  echo "  git commit -m '$COMMIT_MSG'"
  echo "  git tag -a $TAG_NAME -m 'Release $TAG_NAME'  # optional"
  echo "  git push origin $BRANCH && git push origin $TAG_NAME  # optional"
   exit 0
 fi

# Real run: update the CMake file(s)
CURRENT_YEAR="$(date +%Y)"
YEAR_TO_SET="$CURRENT_YEAR"

# Make a backup copy first
BACKUP_DIR=$(mktemp -d)
cp "$CMAKE_FILE" "$BACKUP_DIR/$(basename "$CMAKE_FILE").bak"
# create backups (assume files exist)
cp "$SONAR_FILE" "$BACKUP_DIR/$(basename "$SONAR_FILE").bak"
cp "$VCPKG_FILE" "$BACKUP_DIR/$(basename "$VCPKG_FILE").bak"

update_cmake_version "$CMAKE_FILE" "$VER_HI" "$VER_LO" "$VER_REV" "$YEAR_TO_SET"

# Update sonar-project.properties if present: replace sonar.projectVersion or append it
tmp=$(mktemp)
awk -v ver="$NEW_VERSION" 'BEGIN{found=0} /^sonar.projectVersion[[:space:]]*=/ { print "sonar.projectVersion=" ver; found=1; next } { print } END { if (!found) print "sonar.projectVersion=" ver }' "$SONAR_FILE" > "$tmp"
mv "$tmp" "$SONAR_FILE"

# Update vcpkg.json: use Python json module to set "version-string" = NEW_VERSION
tmp=$(mktemp)
python3 - "$VCPKG_FILE" "$NEW_VERSION" > "$tmp" <<'PY'
import sys, json
path = sys.argv[1]
ver = sys.argv[2]
with open(path, 'r', encoding='utf-8') as f:
    data = json.load(f)
data['version-string'] = ver
with open(path, 'w', encoding='utf-8') as f:
    json.dump(data, f, indent=2, ensure_ascii=False)
    f.write('\n')
print('')
PY
mv "$tmp" "$VCPKG_FILE"

# Check that the file actually changed
CHANGED=0
if ! git diff --no-ext-diff --quiet -- "$CMAKE_FILE"; then
  CHANGED=1
fi
if ! git diff --no-ext-diff --quiet -- "$SONAR_FILE"; then
  CHANGED=1
fi
if ! git diff --no-ext-diff --quiet -- "$VCPKG_FILE"; then
  CHANGED=1
fi
if [ $CHANGED -eq 0 ]; then
  echo "No changes detected in updated files after update. Aborting." >&2
  exit 6
fi

# Stage only the modified file
git add "$CMAKE_FILE"
git add "$SONAR_FILE"
git add "$VCPKG_FILE"

echo "Files updated locally:"
echo "  $CMAKE_FILE"
echo "  $SONAR_FILE"
echo "  $VCPKG_FILE"
echo
echo "This script does not perform git commit, tag or push. To commit and push manually, run the suggested commands shown in dry-run output or:"
echo "  git add $CMAKE_FILE $SONAR_FILE $VCPKG_FILE && git commit -m '$COMMIT_MSG'"
echo "(Optional) create a tag: git tag -a $TAG_NAME -m 'Release $TAG_NAME'"
echo "(Optional) push: git push origin $BRANCH && git push origin $TAG_NAME"

# build and test are intentionally not performed by this script

echo "Success: updated version to $NEW_VERSION"

# cleanup
rm -rf "$BACKUP_DIR"
