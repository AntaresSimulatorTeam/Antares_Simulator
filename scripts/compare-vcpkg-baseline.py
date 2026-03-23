#!/usr/bin/env python3

import argparse
import json
import subprocess
import sys
import tempfile
from pathlib import Path


def get_transitive_deps(vcpkg_dir, direct_deps, triplet=None):
    transitive_deps = set()
    args = [str(vcpkg_dir / "vcpkg"), "depend-info", "--format=list"]
    if triplet:
        args.extend(["--triplet", triplet])

    for dep in direct_deps:
        try:
            result = subprocess.run(
                args + [dep],
                capture_output=True,
                text=True,
                check=True,
            )
            for line in (result.stdout + result.stderr).strip().split("\n"):
                line = line.strip()
                if line and ":" in line:
                    pkg = line.split(":")[0].strip()
                    pkg = pkg.split("[")[0].strip()
                    if pkg != dep:
                        transitive_deps.add(pkg)
        except subprocess.CalledProcessError as e:
            print(f"Warning: Failed to get dependencies for {dep}: {e.returncode}", file=sys.stderr)

    return transitive_deps


def main():
    parser = argparse.ArgumentParser(
        description="Compare vcpkg baseline versions between two commits."
    )
    parser.add_argument("old_commit", help="The old vcpkg commit hash (or short hash)")
    parser.add_argument(
        "new_commit",
        nargs="?",
        default="HEAD",
        help="The new vcpkg commit hash (default: current HEAD)",
    )
    parser.add_argument(
        "vcpkg_json_path",
        nargs="?",
        help="Path to vcpkg.json for direct deps (default: src/vcpkg.json)",
    )
    parser.add_argument(
        "--vcpkg-dir",
        default=None,
        help="Path to vcpkg directory (default: <script_parent>/../vcpkg)",
    )
    parser.add_argument(
        "--triplet",
        default=None,
        help="Target triplet for dependency resolution",
    )

    args = parser.parse_args()

    script_dir = Path(__file__).parent.resolve()
    project_root = script_dir.parent
    vcpkg_dir = args.vcpkg_dir or (project_root / "vcpkg")
    vcpkg_json_path = args.vcpkg_json_path or (project_root / "src" / "vcpkg.json")

    if not vcpkg_json_path or not Path(vcpkg_json_path).is_file():
        print(f"Error: vcpkg.json not found at {vcpkg_json_path}")
        sys.exit(1)

    if not Path(vcpkg_dir).is_dir():
        print(f"Error: vcpkg directory not found at {vcpkg_dir}")
        sys.exit(1)

    if not (vcpkg_dir / ".git").exists():
        print(f"Error: vcpkg directory is not a git repository (not initialized)")
        sys.exit(1)

    print("Comparing vcpkg baseline versions:")
    print(f"  Old: {args.old_commit}")
    print(f"  New: {args.new_commit}")
    print(f"  Project: <project_root>/src/vcpkg.json")
    print()

    with tempfile.TemporaryDirectory() as temp_dir:
        temp_path = Path(temp_dir)

        with open(temp_path / "baseline_old.json", "wb") as f:
            subprocess.run(
                [
                    "git",
                    "-C",
                    vcpkg_dir,
                    "show",
                    f"{args.old_commit}:versions/baseline.json",
                ],
                stdout=f,
                check=True,
            )
        with open(temp_path / "baseline_new.json", "wb") as f:
            subprocess.run(
                [
                    "git",
                    "-C",
                    vcpkg_dir,
                    "show",
                    f"{args.new_commit}:versions/baseline.json",
                ],
                stdout=f,
                check=True,
            )

        with open(temp_path / "baseline_old.json") as f:
            old_data = json.load(f)

        with open(temp_path / "baseline_new.json") as f:
            new_data = json.load(f)

        with open(vcpkg_json_path) as f:
            vcpkg_json = json.load(f)

    old_baseline = old_data.get("default", {})
    new_baseline = new_data.get("default", {})

    direct_deps = set()
    for dep in vcpkg_json.get("dependencies", []):
        if isinstance(dep, dict):
            direct_deps.add(dep.get("name"))
        elif isinstance(dep, str):
            direct_deps.add(dep)

    print("Fetching transitive dependencies...")
    transitive_deps = get_transitive_deps(vcpkg_dir, direct_deps, args.triplet)
    print(f"Found {len(transitive_deps)} transitive dependencies.")
    print()

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
            is_direct = pkg in direct_deps
            is_transitive = pkg in transitive_deps
            changes.append((pkg, old_str, new_str, is_direct, is_transitive))

    if not changes:
        print("No version changes found.")
        sys.exit(0)

    direct_changes = [c for c in changes if c[3]]
    transitive_changes = [c for c in changes if c[4] and not c[3]]

    print("=" * 100)
    print("DIRECT DEPENDENCIES (from vcpkg.json)")
    print("=" * 100)
    if direct_changes:
        print(f"{'Package':<40} {'Old Version':<25} {'New Version':<25}")
        print("-" * 90)
        for pkg, old_ver, new_ver, _, _ in direct_changes:
            print(f"{pkg:<40} {old_ver:<25} {new_ver:<25}")
        print(f"\nTotal direct dependency changes: {len(direct_changes)}")
    else:
        print("No direct dependency changes.")

    print("\n")
    print("=" * 100)
    print("TRANSITIVE DEPENDENCIES (from depend-info)")
    print("=" * 100)
    if transitive_changes:
        print(f"{'Package':<40} {'Old Version':<25} {'New Version':<25}")
        print("-" * 90)
        for pkg, old_ver, new_ver, _, _ in transitive_changes:
            print(f"{pkg:<40} {old_ver:<25} {new_ver:<25}")
        print(f"\nTotal transitive dependency changes: {len(transitive_changes)}")
    else:
        print("No transitive dependency changes.")

    print(
        f"\nTotal changes: {len(changes)} for which {len(direct_changes)} are direct dependencies and {len(transitive_changes)} are transitive dependencies.")


if __name__ == "__main__":
    main()
