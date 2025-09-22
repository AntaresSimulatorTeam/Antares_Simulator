#!/usr/bin/env python3
"""
Relative Path Resolution Tool for Antares Simulator

This utility helps developers resolve and understand relative paths in the codebase.
It can resolve specific paths or analyze files for relative path usage.

Usage:
    python resolve_paths.py --resolve "../path/to/file" --base src/component/file.cpp
    python resolve_paths.py --analyze src/component/file.cpp
    python resolve_paths.py --check-file src/component/file.cpp
"""

import argparse
import os
import re
import sys
from pathlib import Path

def resolve_relative_path(base_file, relative_path, repo_root=None):
    """
    Resolve a relative path given a base file location.
    
    Args:
        base_file: The file containing the relative path
        relative_path: The relative path to resolve
        repo_root: Optional repository root for context
        
    Returns:
        tuple: (resolved_absolute_path, exists, relative_to_repo)
    """
    try:
        # Convert base file to Path object
        base_path = Path(base_file)
        if not base_path.is_absolute():
            if repo_root:
                base_path = Path(repo_root) / base_path
            else:
                base_path = Path.cwd() / base_path
        
        # Resolve the relative path
        base_dir = base_path.parent
        resolved = (base_dir / relative_path).resolve()
        
        # Check if resolved path exists
        exists = resolved.exists()
        
        # Get path relative to repo root if provided
        relative_to_repo = None
        if repo_root:
            try:
                relative_to_repo = resolved.relative_to(Path(repo_root).resolve())
            except ValueError:
                pass  # Path is outside repo
        
        return str(resolved), exists, str(relative_to_repo) if relative_to_repo else None
        
    except Exception as e:
        return None, False, str(e)

def analyze_file_paths(file_path, repo_root=None):
    """
    Analyze a file for relative paths and their resolutions.
    
    Args:
        file_path: Path to the file to analyze
        repo_root: Repository root for context
        
    Returns:
        list: List of dictionaries with path analysis results
    """
    results = []
    
    # Patterns to match relative paths
    patterns = {
        'include': re.compile(r'#include\s*[<"](\.\.?[/\\][^>"]*)[">]'),
        'string_literal': re.compile(r'["\'](\.\.[/\\][^"\']*)["\']'),
        'cmake': re.compile(r'(\.\.[/\\][^\s)]*)')
    }
    
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        for line_num, line in enumerate(lines, 1):
            line_content = line.strip()
            
            for pattern_name, pattern in patterns.items():
                matches = pattern.findall(line_content)
                for match in matches:
                    resolved, exists, relative_to_repo = resolve_relative_path(
                        file_path, match, repo_root
                    )
                    
                    results.append({
                        'line_num': line_num,
                        'line_content': line_content,
                        'relative_path': match,
                        'pattern_type': pattern_name,
                        'resolved_path': resolved,
                        'exists': exists,
                        'relative_to_repo': relative_to_repo
                    })
    
    except Exception as e:
        print(f"Error analyzing file {file_path}: {e}", file=sys.stderr)
    
    return results

def print_analysis_results(results, file_path):
    """Print formatted analysis results."""
    print(f"\n=== Relative Path Analysis for {file_path} ===")
    print(f"Found {len(results)} relative paths:")
    print()
    
    for i, result in enumerate(results, 1):
        print(f"{i}. Line {result['line_num']} ({result['pattern_type']}):")
        print(f"   Relative Path: {result['relative_path']}")
        print(f"   Line Content:  {result['line_content']}")
        print(f"   Resolves To:   {result['resolved_path']}")
        print(f"   Exists:        {'✓' if result['exists'] else '✗'}")
        if result['relative_to_repo']:
            print(f"   Repo Path:     {result['relative_to_repo']}")
        print()

def check_file_validity(file_path, repo_root=None):
    """Check if all relative paths in a file resolve to existing files."""
    results = analyze_file_paths(file_path, repo_root)
    
    total_paths = len(results)
    existing_paths = sum(1 for r in results if r['exists'])
    missing_paths = total_paths - existing_paths
    
    print(f"\n=== Path Validity Check for {file_path} ===")
    print(f"Total relative paths: {total_paths}")
    print(f"Existing paths: {existing_paths}")
    print(f"Missing paths: {missing_paths}")
    
    if missing_paths > 0:
        print(f"\n⚠️  Missing paths:")
        for result in results:
            if not result['exists']:
                print(f"  Line {result['line_num']}: {result['relative_path']} -> {result['resolved_path']}")
        return False
    else:
        print("✓ All relative paths resolve to existing files/directories")
        return True

def find_repo_root(start_path=None):
    """Find the repository root by looking for .git directory."""
    if start_path is None:
        start_path = Path.cwd()
    else:
        start_path = Path(start_path)
    
    current = start_path.resolve()
    while current != current.parent:
        if (current / '.git').exists():
            return str(current)
        current = current.parent
    
    return None

def main():
    parser = argparse.ArgumentParser(description='Resolve and analyze relative paths in Antares Simulator')
    parser.add_argument('--resolve', metavar='PATH', help='Relative path to resolve')
    parser.add_argument('--base', metavar='FILE', help='Base file for path resolution')
    parser.add_argument('--analyze', metavar='FILE', help='Analyze relative paths in a file')
    parser.add_argument('--check-file', metavar='FILE', help='Check if all relative paths in file exist')
    parser.add_argument('--repo-root', metavar='DIR', help='Repository root directory')
    
    args = parser.parse_args()
    
    # Auto-detect repo root if not provided
    repo_root = args.repo_root or find_repo_root()
    if repo_root:
        print(f"Using repository root: {repo_root}")
    else:
        print("Warning: Could not detect repository root")
    
    # Resolve specific path
    if args.resolve:
        if not args.base:
            print("Error: --base is required when using --resolve")
            sys.exit(1)
        
        resolved, exists, relative_to_repo = resolve_relative_path(
            args.base, args.resolve, repo_root
        )
        
        print(f"\n=== Path Resolution ===")
        print(f"Base File:       {args.base}")
        print(f"Relative Path:   {args.resolve}")
        print(f"Resolved To:     {resolved}")
        print(f"Exists:          {'✓' if exists else '✗'}")
        if relative_to_repo:
            print(f"Repo Path:       {relative_to_repo}")
    
    # Analyze file
    elif args.analyze:
        results = analyze_file_paths(args.analyze, repo_root)
        print_analysis_results(results, args.analyze)
    
    # Check file validity
    elif args.check_file:
        check_file_validity(args.check_file, repo_root)
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()