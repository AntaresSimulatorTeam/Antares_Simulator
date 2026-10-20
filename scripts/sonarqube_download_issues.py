#!/usr/bin/env python3
"""Download SonarCloud issues (Antares_Simulator, OPEN/CONFIRMED) and export them to CSV.

The 1st argument (required) is a PR number (digits):
  python3 sonarqube_download_issues.py 3853        # issues of PR 3853 (pullRequest mode)

The SONARCLOUD_TOKEN environment variable must be set with a valid SonarCloud
access token. Get your token here: https://sonarcloud.io/account/access-tokens?tab=personal_tokens

Columns: FILE | LINE NUMBER | SEVERITY | DESCRIPTION
Sorted by descending severity (BLOCKER -> INFO).
"""

import csv
import json
import os
import sys
import urllib.parse
import urllib.request

TOKEN = os.environ.get("SONARCLOUD_TOKEN")
if not TOKEN:
    print("Error: SONARCLOUD_TOKEN environment variable is required", file=sys.stderr)
    sys.exit(1)
BASE_URL = "https://sonarcloud.io"
ORGANIZATION = "antaressimulatorteam"
PROJECT = "AntaresSimulatorTeam_Antares_Simulator"

SEVERITY_ORDER = {"BLOCKER": 0, "CRITICAL": 1, "MAJOR": 2, "MINOR": 3, "INFO": 4}


def fetch_page(params: dict, page: int, page_size: int = 500) -> dict:
    params = {**params, "ps": str(page_size), "p": str(page)}
    url = f"{BASE_URL}/api/issues/search?{urllib.parse.urlencode(params)}"
    req = urllib.request.Request(url, headers={"Authorization": f"Bearer {TOKEN}"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        return json.load(resp)


def main() -> None:
    if len(sys.argv) < 2:
        print(f"Usage: python3 {sys.argv[0]} <PR_NUMBER>", file=sys.stderr)
        sys.exit(1)
    arg = sys.argv[1]
    if not arg.isdigit():
        print(f"Error: argument must be a PR number (digits): {arg}", file=sys.stderr)
        sys.exit(1)
    base_params = {"organization": ORGANIZATION, "projects": PROJECT, "issueStatuses": "OPEN,CONFIRMED"}
    # The number is interpreted by SonarCloud as the pull request id
    params = {**base_params, "pullRequest": arg}
    output = f"sonarcloud_issues_pr{arg}.csv"
    label = f"PR {arg}"

    issues = []
    page = 1
    while True:
        data = fetch_page(params, page)
        issues.extend(data["issues"])
        if len(issues) >= int(data["total"]):
            break
        page += 1

    rows = []
    for issue in issues:
        file = issue["component"].replace(f"{PROJECT}:", "")
        line = issue.get("textRange", {}).get("startLine") or issue.get("line") or ""
        rows.append(
            {
                "FILE": file,
                "LINE NUMBER": line,
                "SEVERITY": issue["severity"],
                "DESCRIPTION": issue["message"],
            }
        )

    rows.sort(
        key=lambda r: (
            SEVERITY_ORDER.get(r["SEVERITY"], 99),
            r["FILE"],
            int(r["LINE NUMBER"]) if str(r["LINE NUMBER"]).isdigit() else 0,
        )
    )

    with open(output, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=["FILE", "LINE NUMBER", "SEVERITY", "DESCRIPTION"])
        writer.writeheader()
        writer.writerows(rows)

    by_severity = {}
    for row in rows:
        by_severity[row["SEVERITY"]] = by_severity.get(row["SEVERITY"], 0) + 1
    summary = ", ".join(f"{by_severity[s]} {s}" for s in SEVERITY_ORDER if s in by_severity)
    print(f"{label}: {len(rows)} issues ({summary}) written to {output}")


if __name__ == "__main__":
    main()