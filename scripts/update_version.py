#!/usr/bin/env python3
"""
Update project version across files:
 - src/CMakeLists.txt (ANTARES_VERSION_HI/LO/REVISION/YEAR and ANTARES_RC)
 - sonar-project.properties (sonar.projectVersion)
 - src/vcpkg.json (version-string)

Features:
 - Update version in multiple formats (base X.Y.Z or with prerelease X.Y.Z-rcN)
 - Auto-increment or set explicit RC number
 - Combine version argument with --rc flag (e.g. "1.2.3 --rc" → "1.2.3-rcN")
 - Dry-run mode to preview changes
 - Atomic file writes with backups
 - Optional automatic git commit with custom message
 - Default commit message: "chore(version): v<version>"

Usage:
  python3 scripts/update_version.py 1.2.3 [--rc [N]] [--dry-run] [-m MSG] [-c]

RC handling:
  --rc        : Increment current ANTARES_RC (or set to 1 if missing)
  --rc N      : Set ANTARES_RC to explicit value N
  version --rc: Auto-increment RC and update version
"""

import argparse
import datetime
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import List

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
    """Parse command-line arguments.

    The --rc flag supports three modes:
    - Omitted: no RC changes
    - --rc alone: increment current ANTARES_RC
    - --rc N: set ANTARES_RC to explicit value N
    """
    p = argparse.ArgumentParser(
        description="Update project version across files (CMakeLists.txt, sonar-project.properties, vcpkg.json)",
        epilog="""
RC (Release Candidate) handling:
  --rc        : Increment current ANTARES_RC value (or create as 1 if missing)
  --rc N      : Set ANTARES_RC to explicit value N
  VERSION --rc: Combine version with auto-incremented RC (e.g. "1.2.3 --rc" → "1.2.3-rcN")

Examples:
  # Update to version 1.2.3
  %(prog)s 1.2.3

  # Update to version 1.2.3 with RC 5 (becomes 1.2.3-rc5)
  %(prog)s 1.2.3 --rc 5

  # Update to version 1.2.3 and auto-increment RC from current value
  %(prog)s 1.2.3 --rc

  # Only increment current RC number (no version change)
  %(prog)s --rc

  # Dry-run to see what would change
  %(prog)s 1.2.3 --dry-run

  # Auto-commit with custom message
  %(prog)s 1.2.3 -m "chore(version): release 1.2.3"
        """,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    p.add_argument("version", nargs="*",
                   help="Version string (X.Y.Z format), optional with --rc")
    p.add_argument("-r", "--rc", nargs="?", const="__INC__", metavar="N",
                   help="RC number: use --rc to increment, --rc N to set explicit value")
    p.add_argument("-d", "--dry-run", action="store_true", help="Show planned changes without modifying files")
    p.add_argument("-c", "--commit", action="store_true", help="Automatically commit changes to git")
    p.add_argument("-m", "--message", metavar="MSG", help="Commit message (implies --commit)")

    args = p.parse_args()
    # Convert variadic version list to single string
    args.version = args.version[0] if args.version else None

    # Normalize rc value: convert string back to marker if needed
    if args.rc is not None and args.rc != "__INC__":
        try:
            int(args.rc)  # validate it's numeric
        except ValueError:
            p.error(f"Invalid value for --rc: must be an integer, got '{args.rc}'")

    return args


def read_text(path: Path) -> str:
    """Read file content as text."""
    return path.read_text(encoding="utf-8")


def write_atomic(path: Path, text: str) -> None:
    """Write file atomically."""
    tmp = Path(tempfile.mktemp(dir=str(path.parent)))
    tmp.write_text(text, encoding="utf-8")
    os.replace(str(tmp), str(path))


def backup_files(paths: List[Path]) -> Path:
    """Create backup directory with copies of files."""
    bakdir = Path(tempfile.mkdtemp(prefix="update_version_backup_"))
    for p in paths:
        try:
            shutil.copy2(p, bakdir / p.name)
        except Exception as e:
            print(f"Warning: failed to backup {p}: {e}", file=sys.stderr)
    return bakdir


def restore_backups(bakdir: Path, paths: List[Path]) -> None:
    """Restore files from backup."""
    for p in paths:
        backup = bakdir / p.name
        if backup.exists():
            try:
                shutil.copy2(str(backup), str(p))
            except Exception as e:
                print(f"Error: failed to restore {p}: {e}", file=sys.stderr)


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
    # Helper to update or insert a CMake variable
    def set_cmake_var(content: str, var_name: str, value: int) -> str:
        pattern = CMAKE_VAR_RE.get(var_name)
        if not pattern:
            return content
        if re.search(pattern, content):
            return re.sub(pattern, f"set(ANTARES_{var_name} {value})", content)
        # Insert after REVISION if var is new
        insert_pattern = r"(set\(ANTARES_VERSION_REVISION\s+\d+\)\n)"
        return re.sub(insert_pattern, rf"\1set(ANTARES_{var_name} {value})\n", content, count=1)

    # Update all version components
    text = re.sub(CMAKE_VAR_RE["HI"], f"set(ANTARES_VERSION_HI {version.major})", text)
    text = re.sub(CMAKE_VAR_RE["LO"], f"set(ANTARES_VERSION_LO {version.minor})", text)
    text = re.sub(CMAKE_VAR_RE["REVISION"], f"set(ANTARES_VERSION_REVISION {version.patch})", text)

    if re.search(CMAKE_VAR_RE["YEAR"], text):
        text = re.sub(CMAKE_VAR_RE["YEAR"], f"set(ANTARES_VERSION_YEAR {year})", text)

    # Update BETA
    if re.search(CMAKE_VAR_RE["BETA"], text):
        text = re.sub(CMAKE_VAR_RE["BETA"], f"set(ANTARES_BETA {version.beta})", text)
    else:
        text = re.sub(
            r"(set\(ANTARES_VERSION_REVISION\s+\d+\)\n)",
            rf"\1set(ANTARES_BETA {version.beta})\n",
            text, count=1
        )

    # Update RC
    if re.search(CMAKE_VAR_RE["RC"], text):
        text = re.sub(CMAKE_VAR_RE["RC"], f"set(ANTARES_RC {version.rc})", text)
    else:
        text = re.sub(
            r"(set\(ANTARES_BETA\s+\d+\)\n)",
            rf"\1set(ANTARES_RC {version.rc})\n",
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

def stage_and_commit(paths: List[Path], message: str, do_commit: bool) -> None:
    """Stage files and optionally commit."""
    subprocess.check_call(["git", "add"] + [str(p) for p in paths])
    if do_commit:
        subprocess.check_call(["git", "commit", "-m", message])
        print("Committed.")
    else:
        print("Files staged. To commit:")
        print(f"  git commit -m '{message}'")


def get_current_rc() -> int:
    """Return current ANTARES_RC value from CMakeLists.txt or 0 if missing."""
    txt = read_text(CMAKE_PATH)
    m = re.search(CMAKE_VAR_RE["RC"], txt)
    if m:
        return int(m.group(1))
    return 0


def handle_rc_only_mode(args) -> None:
    """Handle --rc-only mode: update only ANTARES_RC in CMakeLists."""
    if args.version:
        print(f"Note: version '{args.version}' will be ignored because --rc was used without combining; "
              "only ANTARES_RC will be updated.", flush=True)

    rc = int(args.rc)
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

    # Interpret args.rc which can be:
    #  - None (not provided)
    #  - '__INC__' (user passed --rc with no value -> increment mode)
    #  - numeric string (user passed --rc 7)
    rc_flag = None
    rc_increment = False
    if hasattr(args, 'rc') and args.rc is not None:
        if args.rc == '__INC__':
            rc_increment = True
            rc_flag = None
        else:
            try:
                rc_flag = int(args.rc)
            except ValueError:
                raise SystemExit("Invalid value for --rc: must be an integer if provided")

    # Handle RC-only mode (when --rc provided without a version)
    if (rc_flag is not None or rc_increment) and not args.version:
        if rc_increment:
            args.rc = str(get_current_rc() + 1)
        else:
            args.rc = str(rc_flag)
        handle_rc_only_mode(args)
        return

    # If both version and --rc are provided, merge them into a prerelease string
    if args.version and (rc_flag is not None or rc_increment):
        if rc_increment:
            newrc = get_current_rc() + 1
        else:
            newrc = rc_flag
        args.version = f"{args.version}-rc{int(newrc)}"

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
    year = str(datetime.datetime.now().year)
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

