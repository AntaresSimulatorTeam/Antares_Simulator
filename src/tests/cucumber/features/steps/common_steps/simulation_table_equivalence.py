# Copyright 2007-2026, RTE (https://www.rte-france.com)
# SPDX-License-Identifier: MPL-2.0

# Systematic legacy <-> Simulation Table equivalence checks.
#
# The legacy solver, run with --output=all, writes BOTH the historical
# mc-ind/<year>/... text tree AND the flat simulation table
# (simulation-table-<year>-optim-nb-<n>.csv). For the quantities the legacy
# weekly problem feeds into the table (via FillLegacySimulationTable /
# LegacyExtraOutputs / LegacyNameMapper) the two must agree.
#
# This module walks a declarative mapping table (LEGACY_TO_ST): for every study
# area / thermal cluster / short-term-storage cluster / link it builds the
# mc-ind hourly Series and the matching Simulation Table Series, then asserts
# they are equal within a per-mapping tolerance over every timestep the table
# covers. Nothing is compared against a frozen reference file, so the check
# keeps working while the table format still evolves.
#
# Notes / limitations baked in below:
#   * The final optimisation pass is authoritative: we read
#     simulation-table-<year>-optim-nb-2.csv when it exists (heuristic /
#     integer-fixing pass) and fall back to -optim-nb-1.csv for single-pass
#     studies. optim-nb-1 is the LP relaxation and diverges from mc-ind on
#     unit-commitment studies.
#   * mc-ind hourly columns are printed rounded, so tolerances are generous
#     (the Simulation Table carries full precision, not the mc-ind rounding).
#   * `actual_num_units_on` (ceil) is the one that matches mc-ind NODU; the raw
#     `num_units_on` LP value can be fractional.
#   * basis_status is never compared.

import configparser
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Optional

import numpy as np
import pandas as pd
from behave import then

from common_steps.simulation_table_reader import (
    OutputFormat,
    make_simu_table_reader,
)


# --------------------------------------------------------------------------- #
# Mapping table
# --------------------------------------------------------------------------- #

# source kinds
AREA_VALUES = "area_values"      # mc-ind/<y>/areas/<area>/values-hourly.txt
AREA_DETAILS = "area_details"    # mc-ind/<y>/areas/<area>/details-hourly.txt  (per thermal cluster)
AREA_STS = "area_sts"            # mc-ind/<y>/areas/<area>/details-STstorage-hourly.txt (per sts cluster)
LINK_VALUES = "link_values"      # mc-ind/<y>/links/<a> - <b>/values-hourly.txt


@dataclass(frozen=True)
class Mapping:
    key: str                       # short id, also the selector for the "<key>" step variant
    source: str
    mc_col: str                    # level-0 column name in the mc-ind file
    st_output: str                 # `output` value in the simulation table
    st_component: str              # format string: {area} {cluster} {sts} {origin} {dest}
    mc_sub: Optional[str] = None   # level-1 column name; None -> first sub-column
    atol: float = 0.5
    rtol: float = 1e-3
    transform: Optional[Callable[[pd.Series], pd.Series]] = None  # applied to the mc-ind Series
    # True => derived from a dual; zero on weeks solved as MIP (duals not
    # extracted there). Kept as metadata / for the coverage doc; see
    # docs/developer-guide/simulation-table-e2e-coverage.md
    dual_derived: bool = False


LEGACY_TO_ST = [
    # ---- area balance level ------------------------------------------------ #
    Mapping("unsupplied_energy", AREA_VALUES, "UNSP. ENRG", "unsupplied_energy",
            "{area}_node", mc_sub="MWh"),
    Mapping("spilled_energy", AREA_VALUES, "SPIL. ENRG", "spilled_energy",
            "{area}_node", mc_sub="MWh"),
    Mapping("price", AREA_VALUES, "MRG. PRICE", "price",
            "{area}_node", mc_sub="Euro", atol=1e-2, rtol=1e-4, dual_derived=True),
    Mapping("actual_load", AREA_VALUES, "LOAD", "actual_load",
            "{area}_load", mc_sub="MWh"),

    # ---- thermal clusters ----------------------------------------------------#
    Mapping("generation_power", AREA_DETAILS, "{cluster}", "generation_power",
            "{area}_thermal_{cluster}", mc_sub="MWh"),
    Mapping("actual_num_units_on", AREA_DETAILS, "{cluster}", "actual_num_units_on",
            "{area}_thermal_{cluster}", mc_sub="NODU", atol=0.5, rtol=0.0),

    # ---- short term storage ---------------------------------------------------#
    Mapping("sts_injection", AREA_STS, "{sts}", "injection_power",
            "{area}_short_term_storage_{sts}", mc_sub="P-injection - MW"),
    Mapping("sts_withdrawal", AREA_STS, "{sts}", "withdrawal_power",
            "{area}_short_term_storage_{sts}", mc_sub="P-withdrawal - MW"),
    Mapping("sts_level", AREA_STS, "{sts}", "level",
            "{area}_short_term_storage_{sts}", mc_sub="Levels - MWh"),

    # ---- links -------------------------------------------------------------- #
    Mapping("flow", LINK_VALUES, "FLOW LIN.", "flow",
            "{origin}_{dest}_link", mc_sub="MWh"),
    Mapping("abs_flow", LINK_VALUES, "FLOW LIN.", "abs_flow",
            "{origin}_{dest}_link", mc_sub="MWh", transform=lambda s: s.abs()),
    Mapping("minus_flow", LINK_VALUES, "FLOW LIN.", "minus_flow",
            "{origin}_{dest}_link", mc_sub="MWh", transform=lambda s: -s),
    Mapping("actual_loop_flow", LINK_VALUES, "LOOP FLOW", "actual_loop_flow",
            "{origin}_{dest}_link", mc_sub="MWh"),
]

MAPPING_BY_KEY = {m.key: m for m in LEGACY_TO_ST}


# --------------------------------------------------------------------------- #
# Study-structure enumeration (from the study input/ tree)
# --------------------------------------------------------------------------- #

def _ini_sections(path: Path) -> list:
    if not path.is_file():
        return []
    cp = configparser.ConfigParser()
    cp.optionxform = str
    try:
        cp.read(path, encoding="utf-8")
    except Exception:
        return []
    return list(cp.sections())


def _areas(study_path: Path) -> list:
    f = study_path / "input" / "areas" / "list.txt"
    if not f.is_file():
        return []
    return [line.strip().lower() for line in f.read_text(encoding="utf-8").splitlines() if line.strip()]


def _thermal_clusters(study_path: Path, area: str) -> list:
    # section name is the cluster id as it appears in mc-ind details columns
    return _ini_sections(study_path / "input" / "thermal" / "clusters" / area / "list.ini")


def _sts_clusters(study_path: Path, area: str) -> list:
    return _ini_sections(study_path / "input" / "st-storage" / "clusters" / area / "list.ini")


def _links(study_path: Path, area: str) -> list:
    # input/links/<area>/properties.ini : one section per destination area
    return _ini_sections(study_path / "input" / "links" / area / "properties.ini")


# --------------------------------------------------------------------------- #
# Simulation table loading (final optimisation pass)
# --------------------------------------------------------------------------- #

def _load_final_pass_table(output_path: Path, year_index: int) -> pd.DataFrame:
    """Load the simulation table for one MC year, preferring optim-nb-2."""
    for optim in (2, 1):
        pattern = f"simulation-table-{year_index}-optim-nb-{optim}.csv"
        if any(output_path.glob(pattern)):
            reader = make_simu_table_reader(output_path, OutputFormat.CSV, pattern)
            return reader()
    raise FileNotFoundError(
        f"No simulation-table-{year_index}-optim-nb-*.csv in {output_path}")


# --------------------------------------------------------------------------- #
# Core comparison
# --------------------------------------------------------------------------- #

def _mc_series(df: pd.DataFrame, col: str, sub: Optional[str]) -> Optional[pd.Series]:
    if col not in df.columns.get_level_values(0):
        return None
    block = df[col]
    if sub is not None:
        if sub not in block.columns:
            return None
        raw = block[sub]
    else:
        raw = block.iloc[:, 0]
    return pd.Series(pd.to_numeric(raw, errors="coerce").values,
                     index=range(len(raw)), dtype=float)


def _st_series(st: pd.DataFrame, component: str, output: str, year_index: int) -> Optional[pd.Series]:
    d = st[(st["component"] == component) & (st["output"] == output)]
    if d.empty:
        return None
    if "scenario_index" in d.columns:
        scoped = d[d["scenario_index"] == year_index]
        if not scoped.empty:
            d = scoped
    d = d.dropna(subset=["absolute_time_index"])
    if d.empty:
        return None
    s = d.set_index(d["absolute_time_index"].astype(int))["value"].astype(float)
    return s[~s.index.duplicated(keep="first")].sort_index()


@dataclass
class _Result:
    checked: list = field(default_factory=list)   # (mapping key, component)
    failures: list = field(default_factory=list)  # human readable strings


def _check_one(res: _Result, m: Mapping, component: str, mc: Optional[pd.Series],
               st: Optional[pd.Series]):
    if mc is None or st is None or len(st) == 0:
        return
    if m.transform is not None:
        mc = m.transform(mc)
    idx = [t for t in st.index if 0 <= t < len(mc)]
    if not idx:
        return
    a = st.reindex(idx).to_numpy()
    b = mc.reindex(idx).to_numpy()
    res.checked.append((m.key, component))
    close = np.isclose(a, b, atol=m.atol, rtol=m.rtol, equal_nan=True)
    if close.all():
        return
    bad = np.where(~close)[0]
    sample = ", ".join(
        f"t={idx[i]}: ST={a[i]:.6g} mc-ind={b[i]:.6g} (Δ={abs(a[i] - b[i]):.3g})"
        for i in bad[:5])
    res.failures.append(
        f"[{m.key}] component '{component}' output '{m.st_output}': "
        f"{len(bad)}/{len(idx)} timesteps differ (atol={m.atol}, rtol={m.rtol}). {sample}")


def _run_equivalence(context, year: int, only_key: Optional[str]):
    assert getattr(context, "soh", None) is not None, "no solver output handler on context"
    study_path = Path(context.study_path)
    output_path = Path(context.output_path)
    year_index = year - 1  # mc-ind folders are 1-based, the table's scenario_index is 0-based

    assert context.soh.has_mc_ind_year(year), \
        f"mc-ind/{year:05d} not produced - run the solver with --output=all and year-by-year results"

    st = _load_final_pass_table(output_path, year_index)

    mappings = LEGACY_TO_ST if only_key is None else [MAPPING_BY_KEY[only_key]]
    if only_key is not None:
        assert only_key in MAPPING_BY_KEY, \
            f"unknown mapping key '{only_key}'. Known: {sorted(MAPPING_BY_KEY)}"

    res = _Result()
    areas = _areas(study_path)

    for m in mappings:
        if m.source == AREA_VALUES:
            for area in areas:
                try:
                    df = context.soh.area_values_hourly(area, year)
                except AssertionError:
                    continue
                comp = m.st_component.format(area=area)
                _check_one(res, m, comp,
                           _mc_series(df, m.mc_col, m.mc_sub),
                           _st_series(st, comp, m.st_output, year_index))

        elif m.source == AREA_DETAILS:
            for area in areas:
                clusters = _thermal_clusters(study_path, area)
                if not clusters:
                    continue
                try:
                    df = context.soh.area_details_hourly(area, year)
                except AssertionError:
                    continue
                for cluster in clusters:
                    comp = m.st_component.format(area=area, cluster=cluster)
                    _check_one(res, m, comp,
                               _mc_series(df, m.mc_col.format(cluster=cluster), m.mc_sub),
                               _st_series(st, comp, m.st_output, year_index))

        elif m.source == AREA_STS:
            for area in areas:
                clusters = _sts_clusters(study_path, area)
                if not clusters:
                    continue
                try:
                    df = context.soh.area_sts_details_hourly(area, year)
                except AssertionError:
                    continue
                for sts in clusters:
                    comp = m.st_component.format(area=area, sts=sts)
                    _check_one(res, m, comp,
                               _mc_series(df, m.mc_col.format(sts=sts), m.mc_sub),
                               _st_series(st, comp, m.st_output, year_index))

        elif m.source == LINK_VALUES:
            for origin in areas:
                for dest in _links(study_path, origin):
                    dest_l = dest.lower()
                    try:
                        df = context.soh.link_values_hourly(f"{origin} - {dest_l}", year)
                    except AssertionError:
                        continue
                    comp = m.st_component.format(origin=origin, dest=dest_l)
                    _check_one(res, m, comp,
                               _mc_series(df, m.mc_col, m.mc_sub),
                               _st_series(st, comp, m.st_output, year_index))

    assert res.checked, (
        "legacy<->simulation-table equivalence check was vacuous: no mapped "
        f"quantity could be compared for year {year}"
        + ("" if only_key is None else f" and key '{only_key}'")
        + ". Check study structure / --output=all.")

    assert not res.failures, (
        f"{len(res.failures)} legacy<->simulation-table mismatch(es) "
        f"(checked {len(res.checked)} component/quantity pairs):\n  - "
        + "\n  - ".join(res.failures))


# --------------------------------------------------------------------------- #
# Steps
# --------------------------------------------------------------------------- #

@then('the simulation table matches the legacy mc-ind output for year {year:d}')
def step_st_matches_legacy(context, year):
    _run_equivalence(context, year, only_key=None)


@then('the simulation table matches the legacy mc-ind output for "{key}" in year {year:d}')
def step_st_matches_legacy_one(context, key, year):
    _run_equivalence(context, year, only_key=key)
