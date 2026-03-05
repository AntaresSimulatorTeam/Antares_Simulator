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
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CMAKE_PATH = ROOT / "src" / "CMakeLists.txt"
SONAR_PATH = ROOT / "sonar-project.properties"
VCPKG_PATH = ROOT / "src" / "vcpkg.json"

VERSION_RE = re.compile(r"^(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)(?:[-.].*)?$")


def parse_args():
    p = argparse.ArgumentParser(description="Update project version in files")
    p.add_argument("version", help="New version (X.Y.Z)")
    p.add_argument("-d", "--dry-run", action="store_true", help="Show planned changes")
    p.add_argument("-c", "--commit", action="store_true", help="Commit changes to git")
    p.add_argument("-m", "--message", metavar='MSG', help="Commit message (enables commit)")
    return p.parse_args()


def validate_version(v):
    if not VERSION_RE.match(v):
        raise SystemExit(f"Invalid version format: {v}. Expected X.Y.Z")


def read_text(path: Path):
    return path.read_text(encoding="utf-8")


def write_atomic(path: Path, text: str):
    tmp = Path(tempfile.mktemp(dir=str(path.parent)))
    tmp.write_text(text, encoding="utf-8")
    os.replace(str(tmp), str(path))


def backup_files(paths):
    bakdir = Path(tempfile.mkdtemp(prefix="update_version_backup_"))
    for p in paths:
        try:
            shutil.copy2(p, bakdir / p.name)
        except Exception:
            # if file missing, ignore - caller may recreate
            pass
    return bakdir


def restore_backups(bakdir, paths):
    for p in paths:
        backup = bakdir / p.name
        if backup.exists():
            shutil.copy2(str(backup), str(p))


def update_cmake(text: str, hi: str, lo: str, rev: str, year: str, beta: int = 0, rc: int = 0):
    # Replace numeric version components
    text_new = re.sub(r"set\(ANTARES_VERSION_HI\s+\d+\)", f"set(ANTARES_VERSION_HI {hi})", text)
    text_new = re.sub(r"set\(ANTARES_VERSION_LO\s+\d+\)", f"set(ANTARES_VERSION_LO {lo})", text_new)
    text_new = re.sub(r"set\(ANTARES_VERSION_REVISION\s+\d+\)", f"set(ANTARES_VERSION_REVISION {rev})", text_new)
    # Year may or may not be present; if present replace, otherwise leave as-is
    if re.search(r"set\(ANTARES_VERSION_YEAR\s+\d+\)", text_new):
        text_new = re.sub(r"set\(ANTARES_VERSION_YEAR\s+\d+\)", f"set(ANTARES_VERSION_YEAR {year})", text_new)

    # Update ANTARES_BETA and ANTARES_RC (CMake builds ANTARES_VERSION_TAG from them)
    if re.search(r"set\(ANTARES_BETA\s+\d+\)", text_new):
        text_new = re.sub(r"set\(ANTARES_BETA\s+\d+\)", f"set(ANTARES_BETA {beta})", text_new)
    else:
        # if not present, add right after revision definition
        text_new = re.sub(r"(set\(ANTARES_VERSION_REVISION\s+\d+\)\n)", r"\1set(ANTARES_BETA %d)\n" % beta, text_new, count=1)

    if re.search(r"set\(ANTARES_RC\s+\d+\)", text_new):
        text_new = re.sub(r"set\(ANTARES_RC\s+\d+\)", f"set(ANTARES_RC {rc})", text_new)
    else:
        # insert after ANTARES_BETA
        text_new = re.sub(r"(set\(ANTARES_BETA\s+\d+\)\n)", r"\1set(ANTARES_RC %d)\n" % rc, text_new, count=1)

    return text_new


def update_sonar(text: str, version: str):
    if re.search(r"^sonar\.projectVersion\s*=", text, flags=re.M):
        text_new = re.sub(r"^sonar\.projectVersion\s*=.*$", f"sonar.projectVersion={version}", text, flags=re.M)
    else:
        # append newline if needed
        if not text.endswith("\n"):
            text = text + "\n"
        text_new = text + f"sonar.projectVersion={version}\n"
    return text_new


def update_vcpkg_json(path: Path, version: str):
    # load json, set version-string, dump
    with open(path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    data['version-string'] = version
    return json.dumps(data, indent=2, ensure_ascii=False) + "\n"


def file_changed(original: str, updated: str) -> bool:
    return original != updated


def git_commit(paths, message):
    # stage files
    subprocess.check_call(["git", "add"] + [str(p) for p in paths])
    subprocess.check_call(["git", "commit", "-m", message])


def main():
    args = parse_args()
    validate_version(args.version)
    do_commit = args.commit or (args.message is not None)
    commit_msg = args.message if args.message is not None else f"chore(version): v{args.version}"

    # Read inputs
    cmake_text = read_text(CMAKE_PATH)
    sonar_text = read_text(SONAR_PATH)

    # vcpkg: if file missing or invalid, attempt to restore from HEAD or create minimal
    if not VCPKG_PATH.exists():
        # try git restore from HEAD
        try:
            out = subprocess.check_output(["git", "show", f"HEAD:{VCPKG_PATH.relative_to(ROOT)}"], stderr=subprocess.DEVNULL)
            VCPKG_PATH.write_bytes(out)
        except subprocess.CalledProcessError:
            # create minimal
            VCPKG_PATH.write_text(json.dumps({"version-string": args.version}, indent=2) + "\n", encoding='utf-8')
    else:
        # check valid json
        try:
            with open(VCPKG_PATH, 'r', encoding='utf-8') as f:
                json.load(f)
        except Exception:
            try:
                out = subprocess.check_output(["git", "show", f"HEAD:{VCPKG_PATH.relative_to(ROOT)}"], stderr=subprocess.DEVNULL)
                VCPKG_PATH.write_bytes(out)
            except subprocess.CalledProcessError:
                VCPKG_PATH.write_text(json.dumps({"version-string": args.version}, indent=2) + "\n", encoding='utf-8')

    vcpkg_orig_text = read_text(VCPKG_PATH)

    # Detect prerelease suffix (rcN or betaN) if present
    beta = 0
    rc = 0
    base_version = args.version
    if '-' in args.version:
        base_version, suffix = args.version.split('-', 1)
        m = re.match(r'^(?P<tag>rc|beta)[.-]?(?P<num>\d+)$', suffix, flags=re.I)
        if m:
            tag = m.group('tag').lower()
            num = int(m.group('num'))
            if tag == 'rc':
                rc = num
            elif tag == 'beta':
                beta = num

    # If an rc suffix is present, we update only CMakeLists (internal rc for builds).
    rc_only = (rc != 0)

    # Prepare updates
    hi, lo, rev = VERSION_RE.match(base_version).groups()
    year = str(__import__('datetime').date.today().year)
    new_cmake = update_cmake(cmake_text, hi, lo, rev, year, beta=beta, rc=rc)

    # For RC versions, update sonar and vcpkg with the base version (without the -rcN)
    new_sonar = update_sonar(sonar_text, base_version)
    new_vcpkg = update_vcpkg_json(VCPKG_PATH, base_version)

    # Dry-run: report
    if args.dry_run:
        print("=== DRY RUN ===")
        if file_changed(cmake_text, new_cmake):
            print(f"CMake -> {CMAKE_PATH}: will be updated")
        else:
            print("CMake: no change")
        # Always report sonar and vcpkg planned changes; for rc versions they use base_version
        if file_changed(sonar_text, new_sonar):
            print(f"Sonar -> {SONAR_PATH}: will be updated (to {base_version if rc_only else args.version})")
        else:
            print("Sonar: no change")
        if file_changed(vcpkg_orig_text, new_vcpkg):
            print(f"vcpkg.json -> {VCPKG_PATH}: will be updated (to {base_version if rc_only else args.version})")
        else:
            print("vcpkg.json: no change")
        print()
        print("Would stage files:")
        for p in (CMAKE_PATH, SONAR_PATH, VCPKG_PATH):
            print(" ", p)
        if do_commit:
            print(f"Would commit with message: {commit_msg!r}")
        else:
            print("Would not commit (use --commit or -m to commit)")
        sys.exit(0)

    # Real run: create backups
    bakdir = backup_files([CMAKE_PATH, SONAR_PATH, VCPKG_PATH])

    try:
        # write files atomically only if changed
        if file_changed(cmake_text, new_cmake):
            write_atomic(CMAKE_PATH, new_cmake)
        # Always update sonar and vcpkg if their content changed (for rc use base version)
        if file_changed(sonar_text, new_sonar):
            write_atomic(SONAR_PATH, new_sonar)
        if file_changed(vcpkg_orig_text, new_vcpkg):
            write_atomic(VCPKG_PATH, new_vcpkg)

        # ensure there is a change
        if not any([file_changed(cmake_text, new_cmake), file_changed(sonar_text, new_sonar), file_changed(vcpkg_orig_text, new_vcpkg)]):
            print("No changes detected, aborting.")
            shutil.rmtree(bakdir)
            sys.exit(6)

        # stage
        subprocess.check_call(["git", "add", str(CMAKE_PATH), str(SONAR_PATH), str(VCPKG_PATH)])

        if do_commit:
            git_commit([CMAKE_PATH, SONAR_PATH, VCPKG_PATH], commit_msg)
            print("Committed.")
        else:
            print("Files staged. To commit:")
            print(f"  git commit -m '{commit_msg}'")

    except Exception as e:
        print("Error during update:", e, file=sys.stderr)
        print("Restoring backups...", file=sys.stderr)
        restore_backups(bakdir, [CMAKE_PATH, SONAR_PATH, VCPKG_PATH])
        shutil.rmtree(bakdir)
        raise
    else:
        shutil.rmtree(bakdir)
        print(f"Updated version to {args.version}")


if __name__ == '__main__':
    main()

