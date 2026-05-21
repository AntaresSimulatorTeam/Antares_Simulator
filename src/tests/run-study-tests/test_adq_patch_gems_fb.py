"""
Integration test: adequacy-patch CSR with GEMS flow-based constraints enabled.

Copies the existing CSR study to a temp directory, injects a minimal system.yml
(zero components) and sets enable-gems-fb-constraints = true, then asserts the
solver succeeds and emits the expected info log line.

Run with:
    pytest test_adq_patch_gems_fb.py --antares-simu-path=/path/to/antares-solver -v
"""
import configparser
import shutil
import subprocess
from pathlib import Path

import pytest

_RESOURCES = (
    Path(__file__).parent.parent / "resources" / "Antares_Simulator_Tests_NR"
).resolve()

STUDY_TEMPLATE = (_RESOURCES / "adequacy-patch-CSR" / "adq-patch-CSR-test-case-v01").resolve()

DEMO_GEMS_OVERLAY = (_RESOURCES / "adequacy-patch-CSR" / "adq-patch-gems-fb-demo").resolve()

MINIMAL_SYSTEM_YML = """\
system:
  id: gems_fb_test_system
  description: Minimal system for GEMS FB constraints integration test
  model-libraries: gems_fb_empty_lib
  components: []
"""

MINIMAL_LIBRARY_YML = """\
library:
  id: gems_fb_empty_lib
  description: Empty library for GEMS FB constraints integration test
  port-types: []
  models: []
"""

LOG_MARKER = "[adq-patch] GEMS FB constraints enabled"


@pytest.mark.gems_fb
def test_gems_fb_constraints_flag_off_is_noop(antares_simu_path, tmp_path):
    """Flag=false (default): solver must succeed, no GEMS log line emitted."""
    if not antares_simu_path:
        pytest.skip("--antares-simu-path not provided")
    if not STUDY_TEMPLATE.exists():
        pytest.skip(f"Base study not found: {STUDY_TEMPLATE}")

    study = tmp_path / "study"
    shutil.copytree(STUDY_TEMPLATE, study)

    result = subprocess.run(
        [str(antares_simu_path), "-i", str(study)],
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, f"Solver failed:\n{result.stdout}\n{result.stderr}"
    combined = result.stdout + result.stderr
    assert LOG_MARKER not in combined, "GEMS log line emitted even though flag is false"


@pytest.mark.gems_fb
def test_gems_fb_constraints_flag_on_logs_info(antares_simu_path, tmp_path):
    """Flag=true with a zero-component system: solver succeeds and logs the info line."""
    if not antares_simu_path:
        pytest.skip("--antares-simu-path not provided")
    if not STUDY_TEMPLATE.exists():
        pytest.skip(f"Base study not found: {STUDY_TEMPLATE}")

    study = tmp_path / "study"
    shutil.copytree(STUDY_TEMPLATE, study)

    # Inject the GEMS INI key
    ini_path = study / "settings" / "generaldata.ini"
    cfg = configparser.ConfigParser(strict=False)
    cfg.read(str(ini_path))
    if not cfg.has_section("adequacy patch"):
        cfg.add_section("adequacy patch")
    cfg.set("adequacy patch", "enable-gems-fb-constraints", "true")
    with open(ini_path, "w") as fh:
        cfg.write(fh)

    # Add a minimal (zero-component) GEMS system
    input_dir = study / "input"
    (input_dir / "system.yml").write_text(MINIMAL_SYSTEM_YML)
    lib_dir = input_dir / "model-libraries"
    lib_dir.mkdir(exist_ok=True)
    (lib_dir / "gems-fb-empty.yml").write_text(MINIMAL_LIBRARY_YML)

    result = subprocess.run(
        [str(antares_simu_path), "-i", str(study)],
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, f"Solver failed:\n{result.stdout}\n{result.stderr}"
    combined = result.stdout + result.stderr
    assert LOG_MARKER in combined, (
        f"Expected log line '{LOG_MARKER}' not found in output.\n"
        f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}"
    )


# ---------------------------------------------------------------------------
# Multi-MC scenario-dependent parameter test
# ---------------------------------------------------------------------------

# A minimal GEMS model with one scenario-dependent flow-based constraint:
#   x <= ram_direct_fbc_0001   (x is an internal variable, no area connection)
# The parameter is VARYING_IN_SCENARIO_ONLY so the adapter selects
# tsNumber = mcYear + 1.  The two-column CSV gives distinct RHS per scenario.
_MULTI_MC_LIBRARY_YML = """\
library:
  id: gems_fb_mc_lib
  description: Library for multi-MC scenario-dependent GEMS FB test
  port-types: []
  models:
    - id: flow_bound_model
      description: Adds one flow-based constraint with scenario-dependent RHS
      parameters:
        - id: ram_direct_fbc_0001
          time-dependent: false
          scenario-dependent: true
      variables:
        - id: slack
          lower-bound: 0
          upper-bound: 1e20
          variable-type: continuous
      constraints:
        - id: flow_based_constraint_fbc
          expression: slack <= ram_direct_fbc_0001
"""

_MULTI_MC_SYSTEM_YML = """\
system:
  id: gems_fb_mc_system
  description: Single component for multi-MC GEMS FB test
  model-libraries: gems_fb_mc_lib
  components:
    - id: ccr_mc
      model: gems_fb_mc_lib.flow_bound_model
      parameters:
        - id: ram_direct_fbc_0001
          time-dependent: false
          scenario-dependent: true
          value: ram_direct_fbc_0001
"""

# Two-column tab-separated CSV: column 1 (tsNumber=1/MC0)=100, column 2 (tsNumber=2/MC1)=200.
_RAM_CSV = "100\t200\n"


@pytest.mark.gems_fb
def test_gems_fb_constraints_multi_mc_scenario_dependent(antares_simu_path, tmp_path):
    """2 MC years with scenario-dependent GEMS parameter: solver must succeed for both.

    The unit test `scenario_dependent_parameter_rhs_differs_per_mc_year` verifies that
    rowsForHour(hour, 0) produces rhs=100 while rowsForHour(hour, 1) produces rhs=200.
    This integration test confirms the solver runs to completion with 2 MC years and a
    scenario-dependent GEMS system (no crashes, no assertion failures).
    """
    if not antares_simu_path:
        pytest.skip("--antares-simu-path not provided")
    if not STUDY_TEMPLATE.exists():
        pytest.skip(f"Base study not found: {STUDY_TEMPLATE}")

    study = tmp_path / "study"
    shutil.copytree(STUDY_TEMPLATE, study)

    # Set MC year count to 2
    ini_path = study / "settings" / "generaldata.ini"
    cfg = configparser.ConfigParser(strict=False)
    cfg.read(str(ini_path))
    if not cfg.has_section("general"):
        cfg.add_section("general")
    cfg.set("general", "nbyears", "2")
    if not cfg.has_section("adequacy patch"):
        cfg.add_section("adequacy patch")
    cfg.set("adequacy patch", "enable-gems-fb-constraints", "true")
    with open(ini_path, "w") as fh:
        cfg.write(fh)

    # Inject GEMS system + library
    input_dir = study / "input"
    (input_dir / "system.yml").write_text(_MULTI_MC_SYSTEM_YML)
    lib_dir = input_dir / "model-libraries"
    lib_dir.mkdir(exist_ok=True)
    (lib_dir / "gems-fb-mc.yml").write_text(_MULTI_MC_LIBRARY_YML)

    # Inject scenario-dependent time-series (2 columns: MC0=100, MC1=200)
    ds_dir = input_dir / "data-series"
    ds_dir.mkdir(exist_ok=True)
    (ds_dir / "ram_direct_fbc_0001.csv").write_text(_RAM_CSV)

    result = subprocess.run(
        [str(antares_simu_path), "-i", str(study)],
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, f"Solver failed:\n{result.stdout}\n{result.stderr}"
    combined = result.stdout + result.stderr
    assert LOG_MARKER in combined, (
        f"Expected log line '{LOG_MARKER}' not found in output.\n"
        f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}"
    )


# ---------------------------------------------------------------------------
# Demo study smoke test
# ---------------------------------------------------------------------------


@pytest.mark.gems_fb
def test_gems_fb_demo_study(antares_simu_path, tmp_path):
    """End-to-end smoke test for the adq-patch GEMS FB demo.

    Assembles a runnable study by overlaying the GEMS-specific files from
    adq-patch-gems-fb-demo/ onto the adq-patch-CSR-test-case-v01 base study,
    then verifies:
      - solver exits cleanly (returncode == 0)
      - the GEMS FB enabled log line is emitted
      - adequacy-patch output columns (LMR VIOL., SPIL. ENRG CSR, DTG MRG CSR)
        appear in the area values files written by the solver
    """
    if not antares_simu_path:
        pytest.skip("--antares-simu-path not provided")
    if not STUDY_TEMPLATE.exists():
        pytest.skip(f"Base study not found: {STUDY_TEMPLATE}")
    if not DEMO_GEMS_OVERLAY.exists():
        pytest.skip(f"Demo overlay not found: {DEMO_GEMS_OVERLAY}")

    study = tmp_path / "study"
    shutil.copytree(STUDY_TEMPLATE, study)

    # Overlay the GEMS-specific input files (model-libraries + system.yml)
    shutil.copytree(DEMO_GEMS_OVERLAY / "input", study / "input", dirs_exist_ok=True)

    # Enable the GEMS FB flag in generaldata.ini
    ini_path = study / "settings" / "generaldata.ini"
    cfg = configparser.ConfigParser(strict=False)
    cfg.read(str(ini_path))
    if not cfg.has_section("adequacy patch"):
        cfg.add_section("adequacy patch")
    cfg.set("adequacy patch", "enable-gems-fb-constraints", "true")
    with open(ini_path, "w") as fh:
        cfg.write(fh)

    result = subprocess.run(
        [str(antares_simu_path), "-i", str(study)],
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, f"Solver failed:\n{result.stdout}\n{result.stderr}"

    combined = result.stdout + result.stderr
    assert LOG_MARKER in combined, (
        f"Expected log line '{LOG_MARKER}' not found in output.\n"
        f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}"
    )

    # Verify adq-patch output columns appear in at least one area values file.
    # The solver writes output to study/output/<timestamp>/economy/mc-all/areas/<area>/.
    adq_columns = {"LMR VIOL.", "SPIL. ENRG CSR", "DTG MRG CSR"}
    output_root = study / "output"
    found_columns: set[str] = set()
    for values_file in output_root.rglob("values-annual.txt"):
        try:
            header = values_file.read_text(errors="replace").split("\n", 2)[1]
        except IndexError:
            continue
        for col in adq_columns:
            if col in header:
                found_columns.add(col)
        if found_columns >= adq_columns:
            break

    assert found_columns >= adq_columns, (
        f"Adq-patch output columns not found in solver output.\n"
        f"Expected: {adq_columns}\n"
        f"Found: {found_columns}\n"
        f"Searched under: {output_root}"
    )