#!/usr/bin/env python3
"""
Update project version across files:
 - src/CMakeLists.txt (ANTARES_VERSION_HI/LO/REVISION/YEAR)
 - sonar-project.properties (sonar.projectVersion)
 - src/vcpkg.json (version-string)

Features:
 - dry-run: show planned changes
 - backup of files
 - atomic writes
 - optional git commit (--commit or when -m provided)
 - default commit message: "chore(version): v<version>"
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parents[1]
CMAKE_PATH = ROOT / "src" / "CMakeLists.txt"
SONAR_PATH = ROOT / "sonar-project.properties"
VCPKG_PATH = ROOT / "src" / "vcpkg.json"

# Regex patterns
VERSION_RE = re.compile(r"^(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)(?:[-.].*)?$")
PRERELEASE_RE = re.compile(r"^(?P<tag>rc|beta)[.-]?(?P<num>\d+)$", re.I)
CMAKE_VAR_RE = {
    "HI": r"set\(ANTARES_VERSION_HI\s+(\d+)\)",
    "LO": r"set\(ANTARES_VERSION_LO\s+(\d+)\)",
    "REVISION": r"set\(ANTARES_VERSION_REVISION\s+(\d+)\)",
    "YEAR": r"set\(ANTARES_VERSION_YEAR\s+(\d+)\)",
    "BETA": r"set\(ANTARES_BETA\s+(\d+)\)",
    "RC": r"set\(ANTARES_RC\s+(\d+)\)",
}


@dataclass
class Version:
    """Parse and store version information."""
    full: str
    base: str
    major: str
    minor: str
    patch: str
    beta: int = 0
    rc: int = 0

    @classmethod
    def parse(cls, version_str: str) -> "Version":
        """Parse version string into components."""
        base_version = version_str
        beta, rc = 0, 0

        if "-" in version_str:
            base_version, suffix = version_str.split("-", 1)
            m = PRERELEASE_RE.match(suffix)
            if m:
                tag = m.group("tag").lower()
                num = int(m.group("num"))
                if tag == "rc":
                    rc = num
                else:
                    beta = num

        if not VERSION_RE.match(base_version):
            raise ValueError(f"Invalid version format: {base_version}. Expected X.Y.Z")

        match = VERSION_RE.match(base_version)
        major, minor, patch = match.groups()

        return cls(full=version_str, base=base_version, major=major, minor=minor,
                   patch=patch, beta=beta, rc=rc)

    @property
    def is_rc_only(self) -> bool:
        """Check if this is an RC version."""
        return self.rc != 0


def parse_args():
    """Parse command-line arguments."""
    p = argparse.ArgumentParser(description="Update project version in files")
    p.add_argument("version", nargs="*",
                   help="New version (X.Y.Z) or omitted when using --rc to only set rc number")
    p.add_argument("-r", "--rc", dest="rc_only", type=int,
                   help="Only set the RC number in CMakeLists (updates ANTARES_RC to <n>)")
    p.add_argument("-d", "--dry-run", action="store_true", help="Show planned changes")
    p.add_argument("-c", "--commit", action="store_true", help="Commit changes to git")
    p.add_argument("-m", "--message", metavar="MSG", help="Commit message (enables commit)")

    args = p.parse_args()
    # Convert variadic version list to single string
    args.version = args.version[0] if args.version else None
    return args


def read_text(path: Path) -> str:
    """Read file content as text."""
    return path.read_text(encoding="utf-8")


def write_atomic(path: Path, text: str) -> None:
    """Write file atomically."""
    tmp = Path(tempfile.mktemp(dir=str(path.parent)))
    tmp.write_text(text, encoding="utf-8")
    os.replace(str(tmp), str(path))


def backup_files(paths: list) -> Path:
    """Create backup directory with copies of files."""
    bakdir = Path(tempfile.mkdtemp(prefix="update_version_backup_"))
    for p in paths:
        try:
            shutil.copy2(p, bakdir / p.name)
        except Exception:
            pass
    return bakdir


def restore_backups(bakdir: Path, paths: list) -> None:
    """Restore files from backup."""
    for p in paths:
        backup = bakdir / p.name
        if backup.exists():
            shutil.copy2(str(backup), str(p))


def update_cmake_rc(cmake_text: str, rc: int) -> str:
    """Update only ANTARES_RC in CMakeLists.txt."""
    if re.search(CMAKE_VAR_RE["RC"], cmake_text):
        return re.sub(CMAKE_VAR_RE["RC"], f"set(ANTARES_RC {rc})", cmake_text)

    # Insert after ANTARES_BETA if present, otherwise after REVISION
    if re.search(CMAKE_VAR_RE["BETA"], cmake_text):
        return re.sub(
            r"(set\(ANTARES_BETA\s+\d+\)\n)",
            r"\1set(ANTARES_RC %d)\n" % rc,
            cmake_text, count=1
        )
    return re.sub(
        r"(set\(ANTARES_VERSION_REVISION\s+\d+\)\n)",
        r"\1set(ANTARES_RC %d)\n" % rc,
        cmake_text, count=1
    )


def update_cmake(text: str, version: Version, year: str) -> str:
    """Update CMakeLists.txt with version information."""
    text = re.sub(CMAKE_VAR_RE["HI"], f"set(ANTARES_VERSION_HI {version.major})", text)
    text = re.sub(CMAKE_VAR_RE["LO"], f"set(ANTARES_VERSION_LO {version.minor})", text)
    text = re.sub(CMAKE_VAR_RE["REVISION"], f"set(ANTARES_VERSION_REVISION {version.patch})", text)

    if re.search(CMAKE_VAR_RE["YEAR"], text):
        text = re.sub(CMAKE_VAR_RE["YEAR"], f"set(ANTARES_VERSION_YEAR {year})", text)

    if re.search(CMAKE_VAR_RE["BETA"], text):
        text = re.sub(CMAKE_VAR_RE["BETA"], f"set(ANTARES_BETA {version.beta})", text)
    else:
        text = re.sub(
            r"(set\(ANTARES_VERSION_REVISION\s+\d+\)\n)",
            r"\1set(ANTARES_BETA %d)\n" % version.beta,
            text, count=1
        )

    if re.search(CMAKE_VAR_RE["RC"], text):
        text = re.sub(CMAKE_VAR_RE["RC"], f"set(ANTARES_RC {version.rc})", text)
    else:
        text = re.sub(
            r"(set\(ANTARES_BETA\s+\d+\)\n)",
            r"\1set(ANTARES_RC %d)\n" % version.rc,
            text, count=1
        )

    return text


def update_sonar(text: str, version: str) -> str:
    """Update sonar-project.properties with version."""
    if re.search(r"^sonar\.projectVersion\s*=", text, flags=re.M):
        return re.sub(r"^sonar\.projectVersion\s*=.*$",
                     f"sonar.projectVersion={version}", text, flags=re.M)

    if not text.endswith("\n"):
        text += "\n"
    return text + f"sonar.projectVersion={version}\n"


def update_vcpkg_json(path: Path, version: str) -> str:
    """Update vcpkg.json with version."""
    with open(path, "r", encoding="utf-8") as f:
        data = json.load(f)
    data["version-string"] = version
    return json.dumps(data, indent=2, ensure_ascii=False) + "\n"

def stage_and_commit(paths: list, message: str, do_commit: bool) -> None:
    """Stage files and optionally commit."""
    subprocess.check_call(["git", "add"] + [str(p) for p in paths])
    if do_commit:
        subprocess.check_call(["git", "commit", "-m", message])
        print("Committed.")
    else:
        print("Files staged. To commit:")
        print(f"  git commit -m '{message}'")


def handle_rc_only_mode(args) -> None:
    """Handle --rc-only mode: update only ANTARES_RC in CMakeLists."""
    if args.version:
        print(f"Note: version '{args.version}' will be ignored because --rc was used; "
              "only ANTARES_RC will be updated.", flush=True)

    rc = int(args.rc_only)
    do_commit = args.commit or (args.message is not None)
    commit_msg = args.message if args.message is not None else f"chore(version): rc{rc}"

    cmake_text = read_text(CMAKE_PATH)
    new_cmake = update_cmake_rc(cmake_text, rc)

    if args.dry_run:
        print("=== DRY RUN ===")
        if cmake_text != new_cmake:
            print(f"CMake -> {CMAKE_PATH}: will be updated (ANTARES_RC -> {rc})")
        else:
            print("CMake: no change")
        print()
        print("Files that would be staged:")
        if cmake_text != new_cmake:
            print(" ", CMAKE_PATH)
        else:
            print(" (none)")
        if do_commit:
            print(f"Would commit with message: {commit_msg!r}")
        else:
            print("Would not commit (use --commit or -m to commit)")
        sys.exit(0)

    bakdir = backup_files([CMAKE_PATH])
    try:
        if cmake_text != new_cmake:
            write_atomic(CMAKE_PATH, new_cmake)
            stage_and_commit([CMAKE_PATH], commit_msg, do_commit)
        else:
            print("No changes detected.")
        shutil.rmtree(bakdir)
    except Exception as e:
        print(f"Error during RC update: {e}", file=sys.stderr)
        print("Restoring backups...", file=sys.stderr)
        restore_backups(bakdir, [CMAKE_PATH])
        shutil.rmtree(bakdir)
        raise


def main():
    """Main entry point."""
    args = parse_args()

    # Handle RC-only mode
    if args.rc_only is not None:
        handle_rc_only_mode(args)
        return

    # Normal mode: version required
    if not args.version:
        raise SystemExit("Missing version argument")

    version = Version.parse(args.version)
    do_commit = args.commit or (args.message is not None)
    commit_msg = args.message if args.message is not None else f"chore(version): v{args.version}"

    # Ensure all files exist
    cmake_text = read_text(CMAKE_PATH)
    sonar_text = read_text(SONAR_PATH)
    vcpkg_text = read_text(VCPKG_PATH)

    # Prepare updates
    year = str(__import__("datetime").date.today().year)
    new_cmake = update_cmake(cmake_text, version, year)
    new_sonar = update_sonar(sonar_text, version.base)
    new_vcpkg = update_vcpkg_json(VCPKG_PATH, version.base)

    # Dry-run
    if args.dry_run:
        print("=== DRY RUN ===")
        if cmake_text != new_cmake:
            print(f"CMake -> {CMAKE_PATH}: will be updated")
        else:
            print("CMake: no change")
        if sonar_text != new_sonar:
            print(f"Sonar -> {SONAR_PATH}: will be updated (to {version.base})")
        else:
            print("Sonar: no change")
        if vcpkg_text != new_vcpkg:
            print(f"vcpkg.json -> {VCPKG_PATH}: will be updated (to {version.base})")
        else:
            print("vcpkg.json: no change")
        print()
        print("Would stage files:")
        for p in [CMAKE_PATH, SONAR_PATH, VCPKG_PATH]:
            print(" ", p)
        if do_commit:
            print(f"Would commit with message: {commit_msg!r}")
        else:
            print("Would not commit (use --commit or -m to commit)")
        sys.exit(0)

    # Real run
    bakdir = backup_files([CMAKE_PATH, SONAR_PATH, VCPKG_PATH])
    try:
        files_to_stage = []

        if cmake_text != new_cmake:
            write_atomic(CMAKE_PATH, new_cmake)
            files_to_stage.append(CMAKE_PATH)

        if sonar_text != new_sonar:
            write_atomic(SONAR_PATH, new_sonar)
            files_to_stage.append(SONAR_PATH)

        if vcpkg_text != new_vcpkg:
            write_atomic(VCPKG_PATH, new_vcpkg)
            files_to_stage.append(VCPKG_PATH)

        if not files_to_stage:
            print("No changes detected, aborting.")
            shutil.rmtree(bakdir)
            sys.exit(6)

        stage_and_commit(files_to_stage, commit_msg, do_commit)
        shutil.rmtree(bakdir)
        print(f"Updated version to {args.version}")

    except Exception as e:
        print(f"Error during update: {e}", file=sys.stderr)
        print("Restoring backups...", file=sys.stderr)
        restore_backups(bakdir, [CMAKE_PATH, SONAR_PATH, VCPKG_PATH])
        shutil.rmtree(bakdir)
        raise


if __name__ == "__main__":
    main()

