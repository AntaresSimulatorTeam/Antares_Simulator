#!/bin/bash
set -e

VCPKG_DIR="/home/marechaljas/CLionProjects/Antares_Simulator/vcpkg"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VCPKG_JSON="${PROJECT_ROOT}/src/vcpkg.json"

usage() {
    echo "Usage: $0 <old_commit> [new_commit] [vcpkg_json_path]"
    echo ""
    echo "Compare vcpkg baseline versions between two commits."
    echo ""
    echo "Arguments:"
    echo "  old_commit        The old vcpkg commit hash (or short hash)"
    echo "  new_commit       The new vcpkg commit hash (default: current HEAD)"
    echo "  vcpkg_json_path  Path to vcpkg.json for direct deps (default: src/vcpkg.json)"
    echo ""
    echo "Example:"
    echo "  $0 b322364f 62159a45"
    echo "  $0 b322364f 62159a45 src/vcpkg.json"
    exit 1
}

if [ -z "$1" ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    usage
fi

OLD_COMMIT="$1"
NEW_COMMIT="${2:-HEAD}"
VCPKG_JSON="${3:-$VCPKG_JSON}"

if [ ! -f "$VCPKG_JSON" ]; then
    echo "Error: vcpkg.json not found at $VCPKG_JSON"
    exit 1
fi

echo "Comparing vcpkg baseline versions:"
echo "  Old: $OLD_COMMIT"
echo "  New: $NEW_COMMIT"
echo "  Project: $VCPKG_JSON"
echo ""

TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

git -C "$VCPKG_DIR" show "$OLD_COMMIT:versions/baseline.json" > "$TEMP_DIR/baseline_old.json"
git -C "$VCPKG_DIR" show "$NEW_COMMIT:versions/baseline.json" > "$TEMP_DIR/baseline_new.json"

python3 << EOF
import json
import sys
import re

with open("$TEMP_DIR/baseline_old.json") as f:
    old_data = json.load(f)

with open("$TEMP_DIR/baseline_new.json") as f:
    new_data = json.load(f)

with open("$VCPKG_JSON") as f:
    vcpkg_json = json.load(f)

old_baseline = old_data.get("default", {})
new_baseline = new_data.get("default", {})

direct_deps = set()
for dep in vcpkg_json.get("dependencies", []):
    if isinstance(dep, dict):
        direct_deps.add(dep.get("name"))
    elif isinstance(dep, str):
        direct_deps.add(dep)

all_packages = sorted(set(old_baseline.keys()) | set(new_baseline.keys()))

def get_version_str(baseline_dict, pkg):
    info = baseline_dict.get(pkg, {})
    ver = info.get("baseline", "N/A")
    pv = info.get("port-version", 0)
    return f"{ver}:{pv}" if ver != "N/A" else "N/A"

changes = []
for pkg in all_packages:
    old_str = get_version_str(old_baseline, pkg)
    new_str = get_version_str(new_baseline, pkg)
    
    if old_str != new_str:
        changes.append((pkg, old_str, new_str, pkg in direct_deps))

if not changes:
    print("No version changes found.")
    sys.exit(0)

direct_changes = [c for c in changes if c[3]]
indirect_changes = [c for c in changes if not c[3]]

print("=" * 100)
print("DIRECT DEPENDENCIES (from vcpkg.json)")
print("=" * 100)
if direct_changes:
    print(f"{'Package':<40} {'Old Version':<25} {'New Version':<25}")
    print("-" * 90)
    for pkg, old_ver, new_ver, is_direct in direct_changes:
        print(f"{pkg:<40} {old_ver:<25} {new_ver:<25}")
    print(f"\nTotal direct dependency changes: {len(direct_changes)}")
else:
    print("No direct dependency changes.")

print("\n")
print("=" * 100)
print("ALL DEPENDENCIES (direct + transitive)")
print("=" * 100)
print(f"{'Package':<40} {'Old Version':<25} {'New Version':<25}")
print("-" * 90)
for pkg, old_ver, new_ver, is_direct in changes:
    marker = "*" if is_direct else " "
    print(f"{marker}{pkg:<39} {old_ver:<25} {new_ver:<25}")

print(f"\nTotal changes: {len(changes)} ({len(direct_changes)} direct, {len(indirect_changes)} indirect)")
EOF
