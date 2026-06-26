#!/usr/bin/env python3
"""
Generate a dead-branches report for a GitHub repository.

Usage:
    python3 dead_branches_report.py [--repo OWNER/REPO] [--output FILE] [--token TOKEN]

Requires:
    - git (with remote already configured)
    - GITHUB_TOKEN env var or --token argument (for PR lookups)

Owner/repo defaults to the remote origin of the current git repo.
"""

import argparse
import json
import os
import subprocess
import sys
import urllib.request
import urllib.error
from dataclasses import dataclass, field
from datetime import date, datetime, timedelta
from pathlib import Path


# ---------------------------------------------------------------------------
# Data model
# ---------------------------------------------------------------------------

@dataclass
class Branch:
    name: str
    author: str
    last_real_commit: str      # ISO date string YYYY-MM-DD, or ""
    open_pr: str = ""          # e.g. "#3766" or "" if none

    @property
    def last_date(self) -> date | None:
        try:
            return date.fromisoformat(self.last_real_commit)
        except (ValueError, TypeError):
            return None

    def age_months(self, today: date) -> float | None:
        d = self.last_date
        if d is None:
            return None
        return (today - d).days / 30.44

    def category(self, today: date, active_months: int, dead_months: int) -> str:
        if self.open_pr:
            return "active"
        m = self.age_months(today)
        if m is None:
            return "unknown"
        if m <= active_months:
            return "active"
        if m <= dead_months:
            return "review"
        return "delete"


# ---------------------------------------------------------------------------
# Git helpers
# ---------------------------------------------------------------------------

def run(cmd: list[str], cwd: str | None = None) -> str:
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=cwd)
    return result.stdout.strip()


def git_remote_owner_repo() -> tuple[str, str]:
    url = run(["git", "remote", "get-url", "origin"])
    # https://github.com/OWNER/REPO.git  or  git@github.com:OWNER/REPO.git
    url = url.removesuffix(".git")
    if "github.com/" in url:
        parts = url.split("github.com/")[-1].split("/")
    elif "github.com:" in url:
        parts = url.split("github.com:")[-1].split("/")
    else:
        # Fallback: take the last two path segments (works for proxied remotes too)
        parts = [p for p in url.rstrip("/").split("/") if p]
        if len(parts) < 2:
            sys.exit(f"Cannot parse remote URL: {url}")
        parts = parts[-2:]
    return parts[0], parts[1]


def fetch_and_prune() -> None:
    print("→ git fetch --prune ...", flush=True)
    subprocess.run(["git", "fetch", "--prune"], capture_output=True)


def list_remote_branches(protected: set[str]) -> list[str]:
    raw = run(["git", "branch", "-r"])
    branches = []
    for line in raw.splitlines():
        line = line.strip()
        if line.startswith("origin/HEAD") or not line.startswith("origin/"):
            continue
        name = line.removeprefix("origin/")
        if name not in protected:
            branches.append(name)
    return sorted(branches)


def last_real_commit(branch: str) -> tuple[str, str]:
    """Return (author_name, date_str) of last non-merge commit on branch."""
    out = run([
        "git", "log", "--no-merges", "-1",
        "--format=%an|%ad",
        "--date=short",
        f"origin/{branch}",
    ])
    if "|" in out:
        author, date_str = out.split("|", 1)
        return author.strip(), date_str.strip()
    return "(unknown)", ""


# ---------------------------------------------------------------------------
# GitHub API helpers
# ---------------------------------------------------------------------------

def github_api(path: str, token: str) -> list | dict:
    url = f"https://api.github.com{path}"
    req = urllib.request.Request(url, headers={
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28",
    })
    try:
        with urllib.request.urlopen(req, timeout=15) as resp:
            return json.loads(resp.read())
    except urllib.error.HTTPError as e:
        print(f"  GitHub API error {e.code} for {path}", file=sys.stderr)
        return []


def fetch_open_prs(owner: str, repo: str, token: str) -> dict[str, str]:
    """Return mapping: head_branch → '#NNN' for all open PRs."""
    result: dict[str, str] = {}
    page = 1
    while True:
        prs = github_api(
            f"/repos/{owner}/{repo}/pulls?state=open&per_page=100&page={page}",
            token,
        )
        if not prs:
            break
        for pr in prs:
            head = pr.get("head", {}).get("ref", "")
            number = pr.get("number", "")
            draft = " (draft)" if pr.get("draft") else ""
            if head:
                result[head] = f"#{number}{draft}"
        if len(prs) < 100:
            break
        page += 1
    return result


# ---------------------------------------------------------------------------
# Markdown rendering
# ---------------------------------------------------------------------------

def md_table(headers: list[str], rows: list[list[str]]) -> str:
    lines = ["| " + " | ".join(headers) + " |"]
    lines.append("| " + " | ".join("---" for _ in headers) + " |")
    for row in rows:
        lines.append("| " + " | ".join(str(c) for c in row) + " |")
    return "\n".join(lines)


def age_label(months: float | None) -> str:
    if months is None:
        return "unknown"
    if months < 1:
        return f"~{int(months * 30)} days"
    return f"~{months:.0f} mo"


def render_report(
    branches: list[Branch],
    protected: set[str],
    active_months: int,
    dead_months: int,
    today: date,
    owner: str,
    repo: str,
) -> str:
    active = [b for b in branches if b.category(today, active_months, dead_months) == "active"]
    review = [b for b in branches if b.category(today, active_months, dead_months) == "review"]
    delete = [b for b in branches if b.category(today, active_months, dead_months) == "delete"]
    unknown = [b for b in branches if b.category(today, active_months, dead_months) == "unknown"]

    active_with_pr = [b for b in active if b.open_pr]
    active_no_pr   = [b for b in active if not b.open_pr]

    lines: list[str] = []

    lines.append(f"# Dead Branches Report — {owner}/{repo}")
    lines.append("")
    lines.append(f"**Generated:** {today.isoformat()}  ")
    lines.append(f"**Branches inventoried:** {len(branches)}  ")
    lines.append(f"**Method:** `git log --no-merges -1` — merge commits are excluded; "
                 f"owner = author of last real commit on the branch.")
    lines.append("")
    lines.append("---")
    lines.append("")
    lines.append("## Summary")
    lines.append("")
    lines.append(md_table(
        ["Category", "Count"],
        [
            [f"Active (< {active_months} months, or open PR)", str(len(active))],
            [f"Likely Dead — Review ({active_months}–{dead_months} months)", str(len(review))],
            [f"Safe to Delete (> {dead_months} months)", str(len(delete))],
            ["Unknown (no non-merge commit found)", str(len(unknown))],
        ],
    ))
    lines.append("")
    lines.append(f"> Cutoffs: **Active** = last real commit within {active_months} months. "
                 f"**Safe to Delete** = older than {dead_months} months.")
    lines.append("")
    lines.append("---")
    lines.append("")

    # --- Active ---
    lines.append(f"## 1. Active")
    lines.append("")

    if active_with_pr:
        lines.append("### With Open PR")
        lines.append("")
        rows = sorted(active_with_pr, key=lambda b: b.last_real_commit or "", reverse=True)
        lines.append(md_table(
            ["Branch", "Owner", "Last Real Commit", "PR"],
            [[f"`{b.name}`", b.author, b.last_real_commit, b.open_pr] for b in rows],
        ))
        lines.append("")

    if active_no_pr:
        lines.append("### Recently Active (no open PR)")
        lines.append("")
        rows = sorted(active_no_pr, key=lambda b: b.last_real_commit or "", reverse=True)
        lines.append(md_table(
            ["Branch", "Owner", "Last Real Commit"],
            [[f"`{b.name}`", b.author, b.last_real_commit] for b in rows],
        ))
        lines.append("")

    lines.append("---")
    lines.append("")

    # --- Review ---
    lines.append(f"## 2. Likely Dead — Review")
    lines.append("")
    lines.append(f"Last real commit **{active_months}–{dead_months} months ago**. "
                 "No open PR. Confirm with the owner before deleting.")
    lines.append("")
    rows = sorted(review, key=lambda b: b.last_real_commit or "", reverse=True)
    lines.append(md_table(
        ["Branch", "Owner", "Last Real Commit", "Age"],
        [[f"`{b.name}`", b.author, b.last_real_commit, age_label(b.age_months(today))]
         for b in rows],
    ))
    lines.append("")
    lines.append("---")
    lines.append("")

    # --- Delete ---
    lines.append(f"## 3. Safe to Delete")
    lines.append("")
    lines.append(f"Last real commit **> {dead_months} months ago**. No open PR.")
    lines.append("")
    rows = sorted(delete, key=lambda b: b.last_real_commit or "", reverse=True)
    lines.append(md_table(
        ["Branch", "Owner", "Last Real Commit", "Age"],
        [[f"`{b.name}`", b.author, b.last_real_commit, age_label(b.age_months(today))]
         for b in rows],
    ))
    lines.append("")
    lines.append("---")
    lines.append("")

    # --- Unknown ---
    if unknown:
        lines.append("## 4. Unknown (no non-merge commit found)")
        lines.append("")
        lines.append(md_table(
            ["Branch", "Owner"],
            [[f"`{b.name}`", b.author] for b in unknown],
        ))
        lines.append("")
        lines.append("---")
        lines.append("")

    # --- By owner ---
    lines.append("## Quick Reference by Owner")
    lines.append("")
    owner_map: dict[str, dict[str, list[str]]] = {}
    for b in branches:
        owner_map.setdefault(b.author, {"active": [], "review": [], "delete": [], "unknown": []})
        owner_map[b.author][b.category(today, active_months, dead_months)].append(b.name)

    rows = []
    for o, cats in sorted(owner_map.items(), key=lambda x: -(len(x[1]["delete"]) + len(x[1]["review"]))):
        del_list  = ", ".join(f"`{n}`" for n in sorted(cats["delete"]))  or "—"
        rev_list  = ", ".join(f"`{n}`" for n in sorted(cats["review"]))  or "—"
        rows.append([o, del_list, rev_list])

    lines.append(md_table(
        ["Owner", "Safe to Delete", "Needs Review (3–12 mo)"],
        rows,
    ))
    lines.append("")

    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(description="Generate a dead-branches markdown report.")
    parser.add_argument("--repo", help="OWNER/REPO (default: parsed from git remote origin)")
    parser.add_argument("--output", default="dead-branches-report.md", help="Output file path")
    parser.add_argument("--token", help="GitHub token (default: GITHUB_TOKEN env var)")
    parser.add_argument("--active-months", type=int, default=3,
                        help="Branches newer than this are 'Active' (default: 3)")
    parser.add_argument("--dead-months", type=int, default=12,
                        help="Branches older than this are 'Safe to Delete' (default: 12)")
    parser.add_argument("--protected", nargs="*", default=["develop", "main", "gh-pages"],
                        help="Branch names to always skip (default: develop main gh-pages)")
    parser.add_argument("--no-fetch", action="store_true", help="Skip git fetch --prune")
    args = parser.parse_args()

    token = args.token or os.environ.get("GITHUB_TOKEN", "")

    if args.repo:
        owner, repo = args.repo.split("/", 1)
    else:
        owner, repo = git_remote_owner_repo()

    print(f"→ Repo: {owner}/{repo}")

    protected = set(args.protected)

    if not args.no_fetch:
        fetch_and_prune()

    branch_names = list_remote_branches(protected)
    print(f"→ Found {len(branch_names)} branches (excluding {', '.join(sorted(protected))})")

    # Fetch open PRs if token available
    open_prs: dict[str, str] = {}
    if token:
        print("→ Fetching open PRs from GitHub API ...", flush=True)
        open_prs = fetch_open_prs(owner, repo, token)
        print(f"  {len(open_prs)} open PRs found")
    else:
        print("  (no GITHUB_TOKEN — skipping PR lookup; set it for full report)", file=sys.stderr)

    # Build branch data
    branches: list[Branch] = []
    total = len(branch_names)
    for i, name in enumerate(branch_names, 1):
        print(f"  [{i}/{total}] {name}", end="\r", flush=True)
        author, date_str = last_real_commit(name)
        branches.append(Branch(
            name=name,
            author=author,
            last_real_commit=date_str,
            open_pr=open_prs.get(name, ""),
        ))
    print()  # newline after \r progress

    today = date.today()
    report = render_report(
        branches=branches,
        protected=protected,
        active_months=args.active_months,
        dead_months=args.dead_months,
        today=today,
        owner=owner,
        repo=repo,
    )

    output_path = Path(args.output)
    output_path.write_text(report, encoding="utf-8")
    print(f"→ Report written to {output_path} ({output_path.stat().st_size // 1024} KB)")


if __name__ == "__main__":
    main()
