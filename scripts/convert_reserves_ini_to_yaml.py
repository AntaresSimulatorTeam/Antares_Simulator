#!/usr/bin/env python3
"""Convert reserves.ini files to YAML format.

Usage:
    python convert_reserves_ini_to_yaml.py [root_dir]

If root_dir is given, converts all reserves.ini under that directory.
If omitted, uses the script's own directory.
"""

import os
import re
import sys
from collections import OrderedDict

try:
    import yaml
except ImportError:
    print("Error: PyYAML is required. Install with: pip install pyyaml", file=sys.stderr)
    sys.exit(1)


def _represent_ordereddict(dumper, data):
    return dumper.represent_mapping('tag:yaml.org,2002:map', data.items())


yaml.add_representer(OrderedDict, _represent_ordereddict)


# ---------------------------------------------------------------------------
# Custom INI parser (handles duplicate section names)
# ---------------------------------------------------------------------------

def parse_ini_to_sections(text):
    """Parse INI text into a list of (section_name, OrderedDict) tuples.

    Handles duplicate section names by preserving all occurrences.
    Special-cases the [symmetries] section for bracket-list values.
    """
    sections = []
    current_section = None
    current_data = OrderedDict()

    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith('#') or line.startswith(';'):
            continue

        section_match = re.match(r'^\[(.+)\]$', line)
        if section_match:
            if current_section is not None:
                sections.append((current_section, current_data))
            current_section = section_match.group(1).strip()
            current_data = OrderedDict()
            continue

        if current_section is None:
            continue

        if current_section == 'symmetries':
            match = re.match(r'^(.+?)\s*=\s*\[(.+)\]$', line)
            if match:
                name = match.group(1).strip()
                reserves = [r.strip() for r in match.group(2).split(',')]
                # Always store as list of lists
                if name in current_data:
                    current_data[name].append(reserves)
                else:
                    current_data[name] = [reserves]  # Note: wrap in list
        else:
            match = re.match(r'^(.+?)\s*=\s*(.+)$', line)
            if match:
                key = match.group(1).strip()
                value = match.group(2).strip()
                current_data[key] = value

    if current_section is not None:
        sections.append((current_section, current_data))

    return sections

# ---------------------------------------------------------------------------
# Reserve definitions converter (reserves.ini -> reserves.yml)
# ---------------------------------------------------------------------------

def convert_reserves(ini_path, yml_path):
    """Convert a reserve definition INI to reserves.yml."""
    with open(ini_path, 'r') as f:
        text = f.read()

    if not text.strip():
        output = OrderedDict([('reserves', [])])
        with open(yml_path, 'w') as f:
            yaml.dump(output, f, default_flow_style=False, sort_keys=False)
        return

    sections = parse_ini_to_sections(text)

    global_params = OrderedDict()
    reserves = []

    for section_name, data in sections:
        if section_name == 'globalparameters':
            gp_keys = [
                ('energy-activation-ratio-up', float),
                ('energy-activation-ratio-down', float),
                ('reference-activation-duration-up', int),
                ('reference-activation-duration-down', int),
            ]
            for key, caster in gp_keys:
                if key in data:
                    global_params[key] = caster(data[key])
        else:
            reserve = OrderedDict()
            reserve['name'] = section_name
            reserve['type'] = data['type']
            reserve['failure-cost'] = float(data['failure-cost'])
            reserve['spillage-cost'] = float(data['spillage-cost'])

            for key in ('power-activation-ratio', 'energy-activation-ratio'):
                if key in data:
                    reserve[key] = float(data[key])
            if 'reference-activation-duration' in data:
                reserve['reference-activation-duration'] = int(data['reference-activation-duration'])

            reserves.append(reserve)

    output = OrderedDict()
    if global_params:
        output['global-parameters'] = global_params
    output['reserves'] = reserves

    with open(yml_path, 'w') as f:
        yaml.dump(output, f, default_flow_style=False, sort_keys=False)


# ---------------------------------------------------------------------------
# Participation converter (reserves.ini -> reserve-participations.yml)
# ---------------------------------------------------------------------------

def convert_participation(ini_path, yml_path, sector_type):
    """Convert a participation INI to reserve-participations.yml.

    sector_type: 'thermal', 'st-storage', or 'hydro'
    """
    with open(ini_path, 'r') as f:
        text = f.read()

    if not text.strip():
        output = OrderedDict([('participations', [])])
        with open(yml_path, 'w') as f:
            yaml.dump(output, f, default_flow_style=False, sort_keys=False)
        return

    sections = parse_ini_to_sections(text)

    symmetries = OrderedDict()
    raw_entries = []

    for section_name, data in sections:
        if section_name == 'symmetries':
            symmetries = OrderedDict(data)
            continue

        reserve_name = section_name
        cert = OrderedDict()
        cert['reserve'] = reserve_name

        if sector_type == 'thermal':
            cluster_name = data.get('cluster-name')
            cert['participation-cost'] = float(data['participation-cost'])
            cert['max-power'] = float(data['max-power'])
            if 'max-power-off' in data:
                cert['max-power-off'] = float(data['max-power-off'])
            if 'participation-cost-off' in data:
                cert['participation-cost-off'] = float(data['participation-cost-off'])
        elif sector_type == 'st-storage':
            cluster_name = data.get('cluster-name')
            cert['participation-cost'] = float(data['participation-cost'])
            cert['max-release'] = float(data['max-release'])
            cert['max-store'] = float(data['max-store'])
        else:
            cluster_name = data.get('cluster-name')
            cert['participation-cost'] = float(data['participation-cost'])
            cert['max-release'] = float(data['max-release'])
            cert['max-store'] = float(data['max-store'])

        raw_entries.append((cluster_name, reserve_name, cert))

    if sector_type == 'hydro':
        _write_hydro_participation(yml_path, raw_entries, symmetries)
    else:
        _write_cluster_participation(yml_path, raw_entries, symmetries, sector_type)


def _write_hydro_participation(yml_path, raw_entries, symmetries):
    """Write hydro participation (flat structure under participations:)."""
    certs = []
    for _cluster_name, _reserve_name, cert in raw_entries:
        certs.append(cert)

    entry = OrderedDict()
    if certs:
        entry['certifications'] = certs
    if symmetries:
        symmetry_list = []
        for v in symmetries.values():
            symmetry_list.extend(OrderedDict([('reserves', reserves)]) for reserves in v)
        entry['symmetries'] = symmetry_list

    output = OrderedDict([('participations', entry)])
    with open(yml_path, 'w') as f:
        yaml.dump(output, f, default_flow_style=False, sort_keys=False)

def _write_cluster_participation(yml_path, raw_entries, symmetries, sector_type):
    """Write thermal or st-storage participation (list under participations:)."""
    groups = OrderedDict()
    for cluster_name, _reserve_name, cert in raw_entries:
        if cluster_name not in groups:
            groups[cluster_name] = []
        groups[cluster_name].append(cert)

    participations = []
    for cluster_name, certs in groups.items():
        entry = OrderedDict()
        if sector_type == 'thermal':
            entry['cluster'] = cluster_name
        else:
            entry['storage'] = cluster_name
        entry['certifications'] = certs

        if cluster_name in symmetries:
            # Now symmetries[cluster_name] is always a list of lists
            entry['symmetries'] = [OrderedDict([('reserves', reserves)]) for reserves in symmetries[cluster_name]]
        participations.append(entry)

    for sym_name, sym_reserves in symmetries.items():
        if sym_name not in groups:
            entry = OrderedDict()
            if sector_type == 'thermal':
                entry['cluster'] = sym_name
            else:
                entry['storage'] = sym_name
            entry['certifications'] = []
            # Handle both formats
            if isinstance(sym_reserves[0], list):
                entry['symmetries'] = [OrderedDict([('reserves', reserves)]) for reserves in sym_reserves]
            else:
                entry['symmetries'] = [OrderedDict([('reserves', sym_reserves)])]
            participations.append(entry)

    output = OrderedDict([('participations', participations)])
    with open(yml_path, 'w') as f:
        yaml.dump(output, f, default_flow_style=False, sort_keys=False)


# ---------------------------------------------------------------------------
# File discovery and dispatcher
# ---------------------------------------------------------------------------

SECTOR_PATTERNS = {
    'thermal': 'input/thermal/clusters',
    'st-storage': 'input/st-storage/clusters',
    'hydro': 'input/hydro/common',
}


def classify_ini_path(rel_path):
    """Determine the type of a reserves.ini file from its relative path.

    Returns one of: 'reserves', 'thermal', 'st-storage', 'hydro', 'standalone', or None.
    """
    normalised = rel_path.replace(os.sep, '/')

    if normalised.startswith('reserves_ini_files/'):
        return 'standalone'

    path_parts = normalised.split('/')

    if len(path_parts) >= 2 and path_parts[-1] == 'reserves.ini':
        path_str = '/'.join(path_parts[:-1])

        for sector, pattern in SECTOR_PATTERNS.items():
            if path_str.endswith(pattern) or pattern in path_str:
                return sector

        if path_str.endswith('input/reserves') or 'input/reserves' in path_str:
            return 'reserves'

    return None


def convert_file(ini_path, root_dir):
    """Convert a single reserves.ini file to YAML."""
    rel_path = os.path.relpath(ini_path, root_dir)
    file_type = classify_ini_path(rel_path)

    if file_type is None:
        return

    dir_path = os.path.dirname(ini_path)

    if file_type == 'reserves':
        yml_path = os.path.join(dir_path, 'reserves.yml')
        print(f"  [reserves]     {rel_path} -> reserves.yml")
        convert_reserves(ini_path, yml_path)
    elif file_type == 'standalone':
        stem = os.path.splitext(os.path.basename(ini_path))[0]
        yml_path = os.path.join(dir_path, stem + '.yml')
        print(f"  [standalone]   {rel_path} -> {stem}.yml")
        convert_reserves(ini_path, yml_path)
    elif file_type in ('thermal', 'st-storage', 'hydro'):
        yml_path = os.path.join(dir_path, 'reserve-participations.yml')
        print(f"  [{file_type}]  {rel_path} -> reserve-participations.yml")
        convert_participation(ini_path, yml_path, file_type)


def convert_all(root_dir):
    """Find and convert all reserves.ini / *.ini files under root_dir."""
    for dirpath, _dirnames, filenames in os.walk(root_dir):
        rel_dir = os.path.relpath(dirpath, root_dir)

        if rel_dir == 'reserves_ini_files' or rel_dir.startswith('reserves_ini_files/'):
            for fn in filenames:
                if fn.endswith('.ini') and fn != 'read.me':
                    ini_path = os.path.join(dirpath, fn)
                    convert_file(ini_path, root_dir)
        else:
            if 'reserves.ini' in filenames:
                ini_path = os.path.join(dirpath, 'reserves.ini')
                convert_file(ini_path, root_dir)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

if __name__ == '__main__':
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))
    root = os.path.abspath(root)

    print(f"Converting reserves.ini files under: {root}")
    convert_all(root)
    print("Done.")
