#!/usr/bin/env bash
# Script to bump project version in CMakeLists.txt, sonar-project.properties, and vcpkg.json
# Usage: scripts/update_version.sh [options] <new-version>
set -euo pipefail
IFS=$'\n\t'
readonly PROGNAME="$(basename "$0")"
readonly REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null || echo '.')"
# File paths
readonly CMAKE_FILE="$REPO_ROOT/src/CMakeLists.txt"
readonly SONAR_FILE="$REPO_ROOT/sonar-project.properties"
readonly VCPKG_FILE="$REPO_ROOT/src/vcpkg.json"
# Defaults
DRY_RUN=0
DO_COMMIT=0
COMMIT_MSG=""
TAG_NAME=""
usage() {
  cat <<EOF
Usage: $PROGNAME [options] <new-version>
Updates version in CMakeLists.txt, sonar-project.properties, and vcpkg.json
Options:
  -d, --dry-run        Show planned changes without modifying files
  -c, --commit         Commit updated files automatically
  -m, --message <msg>  Commit message (default: "chore(version): v<new-version>")
                       Note: -m automatically enables --commit
  -h, --help           Show this help
Examples:
  $PROGNAME --dry-run 10.5.0          # Preview changes
  $PROGNAME 10.5.0                     # Update files only
  $PROGNAME -c 10.5.0                  # Update and commit
  $PROGNAME -m "docs: bump" 10.5.0     # Update and commit with custom message
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
    -d|--dry-run)
      DRY_RUN=1; shift;;
    -c|--commit)
      DO_COMMIT=1; shift;;
    -m|--message)
      COMMIT_MSG="$2"; DO_COMMIT=1; shift 2;;
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
  COMMIT_MSG="chore(version): v$NEW_VERSION"
fi
if [ -z "$TAG_NAME" ]; then
  TAG_NAME="v$NEW_VERSION"
fi
# Ensure we're in a git repo
if ! git rev-parse --git-dir >/dev/null 2>&1; then
  echo "Not a git repository (or any of the parent directories)." >&2
  exit 2
fi
# Warn about uncommitted changes but continue
check_git_state() {
  local branch status
  branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "HEAD")
  status=$(git status --porcelain)
  if [ "$branch" = "HEAD" ]; then
    echo "Warning: Detached HEAD detected; continuing with current commit." >&2
  fi
  if [ -n "$status" ]; then
    echo "Warning: Working tree not clean; proceeding. Changed/untracked files:" >&2
    echo "$status" >&2
  fi
  echo "$branch"
}
BRANCH=$(check_git_state)
# Parse version components
IFS='.' read -r VER_HI VER_LO VER_REV <<< "$(echo "$NEW_VERSION" | cut -d'-' -f1)"
# Update CMakeLists version
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
# Dry-run: show planned changes and exit
if [ $DRY_RUN -eq 1 ]; then
  echo "=== DRY RUN MODE ==="
  echo
  echo "Would update $CMAKE_FILE:"
  echo "  ANTARES_VERSION_HI = $VER_HI"
  echo "  ANTARES_VERSION_LO = $VER_LO"
  echo "  ANTARES_VERSION_REVISION = $VER_REV"
  if grep -q "ANTARES_VERSION_YEAR" "$CMAKE_FILE"; then
    echo "  ANTARES_VERSION_YEAR = $(date +%Y)"
  fi
  echo
  if grep -q '^sonar.projectVersion=' "$SONAR_FILE"; then
    CUR_SONAR_VER=$(grep '^sonar.projectVersion=' "$SONAR_FILE" | cut -d'=' -f2-)
    echo "Would update $SONAR_FILE:"
    echo "  sonar.projectVersion: $CUR_SONAR_VER → $NEW_VERSION"
  else
    echo "Would add to $SONAR_FILE:"
    echo "  sonar.projectVersion=$NEW_VERSION"
  fi
  echo
  if grep -q '"version-string"' "$VCPKG_FILE"; then
    CUR_VCPKG_VER=$(grep '"version-string"' "$VCPKG_FILE" | head -n1 | sed -E 's/.*"version-string"\s*:\s*"([^\"]+)".*/\1/')
    echo "Would update $VCPKG_FILE:"
    echo "  version-string: $CUR_VCPKG_VER → $NEW_VERSION"
  else
    echo "Would add to $VCPKG_FILE:"
    echo "  \"version-string\": \"$NEW_VERSION\""
  fi
  echo
  echo "Files to be staged:"
  echo "  $CMAKE_FILE"
  echo "  $SONAR_FILE"
  echo "  $VCPKG_FILE"
  echo
  if [ $DO_COMMIT -eq 1 ]; then
    echo "Would commit with message: '$COMMIT_MSG'"
  else
    echo "Would NOT commit (use -c or -m to commit)"
    echo "Manual commit command:"
    echo "  git commit -m '$COMMIT_MSG'"
  fi
  echo
  echo "Optional next steps:"
  echo "  git tag -a $TAG_NAME -m 'Release $TAG_NAME'"
  echo "  git push origin $BRANCH $TAG_NAME"
  exit 0
fi
# Real run: update files
CURRENT_YEAR="$(date +%Y)"
# Ensure vcpkg.json is valid, restore from git if needed
restore_vcpkg_if_needed() {
  local rel_vcpkg="${VCPKG_FILE#$REPO_ROOT/}"
  if ! python3 -c "import json; json.load(open('$VCPKG_FILE'))" >/dev/null 2>&1; then
    echo "Warning: $VCPKG_FILE invalid or empty, attempting restore from git HEAD..." >&2
    if git show HEAD:"$rel_vcpkg" > "$VCPKG_FILE" 2>/dev/null; then
      echo "Restored $VCPKG_FILE from git HEAD." >&2
    else
      echo "Creating minimal vcpkg.json with version-string=$NEW_VERSION" >&2
      printf '{\n  "version-string": "%s"\n}\n' "$NEW_VERSION" > "$VCPKG_FILE"
    fi
  fi
}
# Create backups
BACKUP_DIR=$(mktemp -d)
cp "$CMAKE_FILE" "$BACKUP_DIR/$(basename "$CMAKE_FILE").bak"
cp "$SONAR_FILE" "$BACKUP_DIR/$(basename "$SONAR_FILE").bak"
cp "$VCPKG_FILE" "$BACKUP_DIR/$(basename "$VCPKG_FILE").bak"
restore_vcpkg_if_needed
# Update backup after potential restore
cp "$VCPKG_FILE" "$BACKUP_DIR/$(basename "$VCPKG_FILE").bak"
update_cmake_version "$CMAKE_FILE" "$VER_HI" "$VER_LO" "$VER_REV" "$CURRENT_YEAR"
# Update sonar-project.properties
tmp=$(mktemp)
awk -v ver="$NEW_VERSION" 'BEGIN{found=0} /^sonar.projectVersion[[:space:]]*=/ { print "sonar.projectVersion=" ver; found=1; next } { print } END { if (!found) print "sonar.projectVersion=" ver }' "$SONAR_FILE" > "$tmp"
mv "$tmp" "$SONAR_FILE"
# Update vcpkg.json
tmp=$(mktemp)
if python3 - "$VCPKG_FILE" "$NEW_VERSION" > "$tmp" <<'PY'
import sys, json
inpath = sys.argv[1]
ver = sys.argv[2]
with open(inpath, 'r', encoding='utf-8') as f:
    data = json.load(f)
data['version-string'] = ver
sys.stdout.write(json.dumps(data, indent=2, ensure_ascii=False))
sys.stdout.write('\n')
PY
then
  if python3 -c "import json; json.load(open('$tmp'))" >/dev/null 2>&1; then
    mv "$tmp" "$VCPKG_FILE"
  else
    echo "Error: Python produced invalid JSON. Restoring original vcpkg.json." >&2
    mv "$BACKUP_DIR/$(basename "$VCPKG_FILE").bak" "$VCPKG_FILE" 2>/dev/null || true
    rm -f "$tmp"
    exit 7
  fi
else
  echo "Error: Failed to update $VCPKG_FILE" >&2
  mv "$BACKUP_DIR/$(basename "$VCPKG_FILE").bak" "$VCPKG_FILE" 2>/dev/null || true
  rm -f "$tmp"
  exit 7
fi
# Check if files changed
CHANGED=0
for file in "$CMAKE_FILE" "$SONAR_FILE" "$VCPKG_FILE"; do
  if ! git diff --no-ext-diff --quiet -- "$file"; then
    CHANGED=1
    break
  fi
done
if [ $CHANGED -eq 0 ]; then
  echo "No changes detected. Aborting." >&2
  rm -rf "$BACKUP_DIR"
  exit 6
fi
# Stage files
git add "$CMAKE_FILE" "$SONAR_FILE" "$VCPKG_FILE"
echo "Files updated:"
echo "  $CMAKE_FILE"
echo "  $SONAR_FILE"
echo "  $VCPKG_FILE"
echo
# Commit if requested
if [ $DO_COMMIT -eq 1 ]; then
  echo "Committing changes..."
  if git commit -m "$COMMIT_MSG" -- "$CMAKE_FILE" "$SONAR_FILE" "$VCPKG_FILE"; then
    echo "✓ Committed as $(git rev-parse --short HEAD)"
  else
    echo "Error: Commit failed. Restoring backups." >&2
    mv "$BACKUP_DIR/$(basename "$CMAKE_FILE").bak" "$CMAKE_FILE" 2>/dev/null || true
    mv "$BACKUP_DIR/$(basename "$SONAR_FILE").bak" "$SONAR_FILE" 2>/dev/null || true
    mv "$BACKUP_DIR/$(basename "$VCPKG_FILE").bak" "$VCPKG_FILE" 2>/dev/null || true
    git reset -- "$CMAKE_FILE" "$SONAR_FILE" "$VCPKG_FILE" 2>/dev/null || true
    rm -rf "$BACKUP_DIR"
    exit 7
  fi
else
  echo "Files staged. To commit:"
  echo "  git commit -m '$COMMIT_MSG'"
fi
echo
echo "Next steps (optional):"
echo "  git tag -a $TAG_NAME -m 'Release $TAG_NAME'"
echo "  git push origin $BRANCH $TAG_NAME"
echo
echo "✓ Version updated to $NEW_VERSION"
rm -rf "$BACKUP_DIR"
