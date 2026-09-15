#!/usr/bin/env python3
"""Télécharge les issues SonarCloud (Antares_Simulator, OPEN/CONFIRMED) et les exporte en CSV.

Le 1er argument (obligatoire) est soit un numéro de PR (chiffres), soit un nom de branche :
  python3 sq_download_issues.py 3853               # issues de la PR 3853 (mode pullRequest)
  python3 sq_download_issues.py develop            # issues de la branche develop

Colonnes : FILE | LINE NUMBER | SEVERITY | DESCRIPTION
Tri par sévérité décroissante (BLOCKER -> INFO).
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
        print(f"Usage : python3 {sys.argv[0]} <PR_NUMBER | BRANCH_NAME>", file=sys.stderr)
        sys.exit(1)
    arg = sys.argv[1]
    base_params = {"organization": ORGANIZATION, "projects": PROJECT, "issueStatuses": "OPEN,CONFIRMED"}
    if arg.isdigit():
        # Mode PR : le numéro est interprété par SonarCloud comme l'id de pull request
        params = {**base_params, "pullRequest": arg}
        output = f"sonarcloud_issues_pr{arg}.csv"
        label = f"PR {arg}"
    else:
        # Mode branche
        params = {**base_params, "branch": arg}
        output = f"sonarcloud_issues_{arg.replace('/', '_')}.csv"
        label = f"branche {arg}"

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
    print(f"{label} : {len(rows)} issues ({summary}) écrites dans {output}")


if __name__ == "__main__":
    main()