#!/usr/bin/env python3
import re
import sys
from pathlib import Path

R = Path(__file__).resolve().parent.parent
S = R / "src"

def v(text):
    m = re.fullmatch(r"(\d+)\.(\d+)\.(\d+)(-rc\d+|-beta\d+)?", text)
    if not m:
        raise SystemExit(f"bad version: {text}")
    a, b, c, tag = m.groups()
    return int(a), int(b), int(c), tag or ""

def t(path): return path.read_text()
def w(path, text): path.write_text(text)

def cur():
    c = t(S / "CMakeLists.txt")
    return tuple(map(int, [re.search(r"HI (\d+)", c).group(1), re.search(r"LO (\d+)", c).group(1), re.search(r"REVISION (\d+)", c).group(1)]))

def apply(path, rules):
    s = t(path)
    for a, b in rules:
        s, n = re.subn(a, b, s, flags=re.M, count=1)
        if n != 1:
            raise SystemExit(f"{path}: no match for {a}")
    w(path, s)

def bump_version_cpp(major, minor):
    p = S / "libs/antares/study/version.cpp"
    s = t(p)
    if f"StudyVersion({major}, {minor})" not in s:
        s = s.replace("  // Add new versions here", f"  StudyVersion({major}, {minor}),\n  // Add new versions here", 1)
        w(p, s)

def print_cmd():
    c = t(S / "libs/antares/study/version.cpp")
    vs, on = [], False
    for line in c.splitlines():
        on = on or "supportedVersions" in line
        if on and "});" in line:
            break
        m = re.search(r"StudyVersion\((\d+), (\d+)\)", line)
        if m:
            vs.append(f"{m.group(1)}.{m.group(2)}")
    a, b, c = cur()
    print(f"version: {a}.{b}.{c}")
    print(f"sonar.projectVersion: {a}.{b}.{c}")
    print(f"supported study versions: {', '.join(vs)}")

def stage_cmd(vn):
    a, b, c, tag = v(vn)
    apply(R / "sonar-project.properties", [(r"^sonar\.projectVersion=.*$", f"sonar.projectVersion={a}.{b}.{c}{tag}")])
    apply(S / "CMakeLists.txt", [
        (r"^set\(ANTARES_VERSION_HI \d+\)$", f"set(ANTARES_VERSION_HI {a})"),
        (r"^set\(ANTARES_VERSION_LO \d+\)$", f"set(ANTARES_VERSION_LO {b})"),
        (r"^set\(ANTARES_VERSION_REVISION \d+\)$", f"set(ANTARES_VERSION_REVISION {c})"),
        (r"^set\(ANTARES_BETA \d+\)$", f"set(ANTARES_BETA {int(tag.startswith('-beta') and tag[5:] or 0)})"),
        (r"^set\(ANTARES_RC \d+\)$", f"set(ANTARES_RC {int(tag.startswith('-rc') and tag[3:] or 0)})"),
    ])
    apply(S / "vcpkg.json", [(r'^  "version-string": ".*",$', f'  "version-string": "{a}.{b}.{c}",')])
    if a > cur()[0]:
        bump_version_cpp(a, b)

def main(argv):
    if len(argv) == 2 and argv[1] == "print":
        print_cmd()
        return 0
    if len(argv) == 3 and argv[1] == "stage":
        stage_cmd(argv[2])
        return 0
    print("usage: antares-version.py print|stage <version>", file=sys.stderr)
    return 2

raise SystemExit(main(sys.argv))
