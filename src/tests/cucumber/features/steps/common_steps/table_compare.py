import csv
import io
from typing import Optional


def parse_csv_lines(lines):
    if lines is None:
        return None
    reader = csv.DictReader(io.StringIO("".join(lines)))
    return list(reader)


def key_of_row(row: dict):
    # Stable key for identifying a row
    keys = [
        "block",
        "component",
        "output",
        "absolute_time_index",
        "block_time_index",
        "scenario_index",
    ]
    if all(k in row for k in keys):
        return tuple((row[k] or "").strip() for k in keys)
    # Fallback: all values ordered by column name
    colnames = sorted(k for k in row.keys() if k is not None)
    return ("__all__", tuple((k, (row[k] or "").strip()) for k in colnames))


def index_rows(rows):
    index = {}
    for r in rows:
        index.setdefault(key_of_row(r), []).append(r)
    return index


def as_number(s: str):
    try:
        return float(s)
    except Exception:
        return None


def compare_rows(r_ref: dict, r_out: dict):
    diffs = {}
    # Compare common columns; also report missing columns
    ref_cols = {k for k in r_ref.keys() if k}
    out_cols = {k for k in r_out.keys() if k}
    common = ref_cols & out_cols
    for c in sorted(common):
        a_raw = r_ref[c] if r_ref[c] is not None else ""
        b_raw = r_out[c] if r_out[c] is not None else ""
        a = a_raw.strip()
        b = b_raw.strip()
        if a == b:
            continue
        a_num = as_number(a)
        b_num = as_number(b)
        if a_num is not None and b_num is not None:
            # Numeric compare to avoid false positives like '1' vs '1.0'
            if a_num != b_num:
                diffs[c] = (a_raw, b_raw)
        else:
            diffs[c] = (a_raw, b_raw)
    missing_in_out = sorted(ref_cols - out_cols)
    if missing_in_out:
        diffs["__missing_in_out__"] = missing_in_out
    return diffs


def format_key(key_tuple):
    # Format a readable key
    labels = [
        "block",
        "component",
        "output",
        "absolute_time_index",
        "block_time_index",
        "scenario_index",
    ]
    if key_tuple and key_tuple[0] == "__all__":
        return "(raw-row-key from all columns)"
    parts = []
    for i, v in enumerate(key_tuple):
        if i < len(labels):
            parts.append(f"{labels[i]}={v}")
        else:
            parts.append(str(v))
    return ", ".join(parts)


def diff_message(name: str, ref_lines, out_lines) -> Optional[str]:
    ref_rows = parse_csv_lines(ref_lines)
    out_rows = parse_csv_lines(out_lines)
    if ref_rows is None and out_rows is None:
        return None
    if ref_rows is None:
        return f"{name}: reference is missing but output exists"
    if out_rows is None:
        return f"{name}: output is missing but reference exists"

    ref_index = index_rows(ref_rows)
    out_index = index_rows(out_rows)

    ref_keys = set(ref_index.keys())
    out_keys = set(out_index.keys())

    missing_keys = sorted(ref_keys - out_keys)
    extra_keys = sorted(out_keys - ref_keys)

    msg_lines = []
    if missing_keys:
        msg_lines.append(f"- Missing rows in output (samples):")
        for k in missing_keys[:10]:
            msg_lines.append(f"  * {_format_key(k)}")
        if len(missing_keys) > 10:
            msg_lines.append(f"  ... and {len(missing_keys) - 10} more")
    if extra_keys:
        msg_lines.append(f"- Unexpected extra rows (samples):")
        for k in extra_keys[:10]:
            msg_lines.append(f"  * {_format_key(k)}")
        if len(extra_keys) > 10:
            msg_lines.append(f"  ... and {len(extra_keys) - 10} more")

    # Compare lines present in both
    mismatches_shown = 0
    mismatch_limit = 50
    for k in sorted(ref_keys & out_keys):
        ref_list = ref_index[k]
        out_list = out_index[k]
        # If multiple, compare in appearance order
        count = min(len(ref_list), len(out_list))
        for i in range(count):
            diffs = compare_rows(ref_list[i], out_list[i])
            if diffs:
                if mismatches_shown == 0:
                    msg_lines.append("- Different values (samples):")
                mismatches_shown += 1
                if mismatches_shown <= mismatch_limit:
                    msg_lines.append(f"  * {_format_key(k)}:")
                    for col, pair in diffs.items():
                        if col == "__missing_in_out__":
                            msg_lines.append(f"      columns missing in output: {pair}")
                        else:
                            msg_lines.append(f"      {col}: ref='{pair[0]}' vs out='{pair[1]}'")
        # If different counts for the same key, report
        if len(ref_list) != len(out_list):
            msg_lines.append(
                f"  * {_format_key(k)}: occurrence count ref={len(ref_list)} vs out={len(out_list)}")

        if mismatches_shown >= mismatch_limit:
            msg_lines.append("  ... display limit reached")
            break

    if msg_lines:
        header = f"{name} does not match reference"
        return header + "\n" + "\n".join(msg_lines)
    return None
