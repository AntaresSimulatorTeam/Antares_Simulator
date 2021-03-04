from pathlib import Path
import urllib.request
import zipfile
import os
import sys
import glob
import shutil

import numpy as np
import subprocess

import pytest

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests')

@pytest.fixture(scope="session", autouse=True)
def prepare_studies(request):
    if os.path.isdir(ALL_STUDIES_PATH):
        print("Study files found, no need to download")
    else:
        # TODO Find a better host, download speed from this source is limited to ~2.5Mb/s as of December 2020
        url = 'https://github.com/AntaresSimulatorTeam/Antares_Simulator_Tests/archive/master.zip'
        archive = 'master.zip'
        print("Downloading studies")
        urllib.request.urlretrieve(url, archive)
        print("Unzipping studies")
        with zipfile.ZipFile(archive, 'r') as zip_ref:
            zip_ref.extractall('.')
        os.remove(archive)
        print("Done")


def find_integrity_path(output_dir):
    op = []
    for path in Path(output_dir).rglob('checkIntegrity.txt'):
        op.append(path)
    assert len(op) == 1
    return op[0]

def find_values_hourly_path(output_dir, area, year):
    op = []
    for path in Path(output_dir).rglob(f'mc-ind/{year:05d}/areas/{area}/values-hourly.txt'):
        op.append(path)
    assert len(op) == 1
    return op[0]

def fetch_hourly_column_values(path, area, year, col_index):
    output_path = path / 'output'
    hourly_path = find_values_hourly_path(output_path, area, year)
    # skiprows=7 in order to skip the header of the file
    col_values = np.loadtxt(hourly_path, delimiter='\t', skiprows=7, usecols=col_index, dtype=float)
    assert len(col_values == 364 * 24)
    return col_values

def get_integrity_check_values(output : Path) -> np.array :
    integrity_path = find_integrity_path(output)
    integrity_file = open(str(integrity_path), 'r')
    output_values = list()
    for x in integrity_file:
        output_values.append(float(x))
    assert len(output_values) == 8
    return output_values

def remove_outputs(study_path):
    output_path = study_path / 'output'
    files = glob.glob(str(output_path))
    for f in files:
        shutil.rmtree(f)

def launch_solver(solver_path, study_path, use_ortools = False, ortools_solver = "sirius"):
    # Clean study output
    remove_outputs(study_path)
    
    solver_path_full = str(Path(solver_path).resolve())

    command = [solver_path_full, "-i", str(study_path)]
    if use_ortools:
        command.append('--use-ortools')
        command.append('--ortools-solver='+ortools_solver)
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=None)
    output = process.communicate()

    # TODO check return value
    assert "Solver returned error" not in output[0].decode('utf-8')


def check_integrity_first_opt(path):
    reference_path = path / 'reference'
    reference_values = get_integrity_check_values(reference_path)

    output_path = path / 'output'
    output_values = get_integrity_check_values(output_path)

    np.testing.assert_allclose(reference_values[0:4], output_values[0:4], rtol=1e-3, atol=0)

def check_integrity_second_opt(path):
    reference_path = path / 'reference'
    reference_values = get_integrity_check_values(reference_path)

    output_path = path / 'output'
    output_values = get_integrity_check_values(output_path)

    np.testing.assert_allclose(reference_values[4:8], output_values[4:8], rtol=1e-3, atol=0)

def generate_reference_integrity(solver_path, path):
    reference_path = path / 'reference'
    os.makedirs(reference_path, exist_ok=True)
    launch_solver(solver_path,path)

    output_path = path / 'output'
    integrity_path = find_integrity_path(output_path)
    shutil.copy2(integrity_path, reference_path / 'checkIntegrity.txt')

def run_study(solver_path, path, use_ortools, ortools_solver):
    # Launch antares-solver
    launch_solver(solver_path, path, use_ortools, ortools_solver)

## TESTS ##
@pytest.mark.short
def test_001_one_node_passive(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_002_thermal_fleet_base(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_003_thermal_fleet_must_run(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "003 Thermal fleet - Must-run"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_004_thermal_fleet_partial_must_run(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "004 Thermal fleet - Partial must-run"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_005_thermal_fleet_minimum_stable_power_and_min_up_down_times(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "005 Thermal fleet - Minimum stable power and min up down times"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_006_thermal_fleet_extra_costs(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "006 Thermal fleet - Extra costs"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_007_thermal_fleet_fast_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "007 Thermal fleet - Fast unit commitment"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_008_thermal_fleet_accurate_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "008 Thermal fleet - Accurate unit commitment"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_009_ts_generation_thermal_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "009 TS generation - Thermal power"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_010_ts_generation_wind_speed(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "010 TS generation - Wind speed"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_011_ts_generation_wind_power_small_scale(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "011 TS generation - Wind power - small scale"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_012_ts_generation_wind_power_large_scale(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "012 TS Generation - Wind power - large scale"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_013_ts_generation_solar_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "013 TS Generation - Solar power"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_014_ts_generation_load(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "014 TS generation - Load"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_015_ts_generation_hydro_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "015 TS generation - Hydro power"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_016_probabilistic_vs_deterministic_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "016 Probabilistic vs deterministic - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_018_probabilistic_vs_deterministic_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "018 Probabilistic vs deterministic - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_020_single_mesh_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "020 Single mesh - DC law"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_021_four_areas_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "021 Four areas - DC law"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_022_negative_marginal_price(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "022 Negative marginal price"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_023_anti_pricewise_flows(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "023 Anti-pricewise flows"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_024_hurdle_costs_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "024 Hurdle costs - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_025_hurdle_costs_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "025 Hurdle costs - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_026_day_ahead_reserve_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "026 Day ahead reserve - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_027_day_ahead_reserve_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "027 Day ahead reserve - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_028_pumped_storage_plant_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "028 Pumped storage plant - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_029_pumped_storage_plant_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "029 Pumped storage plant - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_030_pumped_storage_plant_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "030 Pumped storage plant - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_031_wind_analysis(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "031 Wind Analysis"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_033_mixed_expansion_storage(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "033 Mixed Expansion - Storage"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_034_mixed_expansion_smart_grid_model_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "034 Mixed Expansion - Smart grid model 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_036_multistage_study_1_isolated_systems(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "036 Multistage study -1-Isolated systems"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_037_multistage_study_2_copperplate(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "037 Multistage study-2-Copperplate"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_040_multistage_study_5_derated(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "040 Multistage study-5-Derated"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_044_psp_strategies_1_no_psp(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "044 PSP strategies-1-No PSP"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_045_psp_strategies_2_det_pumping(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "045 PSP strategies-2-Det pumping"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_046_psp_strategies_3_opt_daily(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "046 PSP strategies-3-Opt daily"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_047_psp_strategies_4_opt_weekly(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "047 PSP strategies-4-Opt weekly"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_053_system_map_editor_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "053 System Map Editor - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_054_system_map_editor_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "054 System Map Editor -2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_055_system_map_editor_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "055 System Map Editor - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_056_system_map_editor_4(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "056 System Map Editor - 4"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_057_four_areas_grid_outages_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "057 Four areas - Grid outages  01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_058_four_areas_grid_outages_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "058 Four areas - Grid outages  02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_059_four_areas_grid_outages_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "059 Four areas - Grid outages  03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_060_four_areas_grid_outages_04(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "060 Four areas - Grid outages  04"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_061_four_areas_grid_outages_05(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "061 Four areas - Grid outages  05"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_062_grid_topology_changes_on_contingencies_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "062 Grid-Topology changes on contingencies 01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_063_grid_topology_changes_on_contingencies_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "063 Grid-Topology changes on contingencies 02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_064_probabilistic_exchange_capacity(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "064 Probabilistic exchange capacity"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_065_pumped_storage_plant_explicit_model_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "065 Pumped storage plant -explicit model-01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_066_pumped_storage_plant_explicit_model_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "066 Pumped storage plant -explicit model-02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_067_pumped_storage_plant_explicit_model_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "067 Pumped storage plant -explicit model-03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_068_hydro_reservoir_model_enhanced_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "068 Hydro Reservoir Model -enhanced-01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_069_hydro_reservoir_model_enhanced_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "069 Hydro Reservoir Model -enhanced-02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_070_hydro_reservoir_model_enhanced_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "070 Hydro Reservoir Model -enhanced-03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform == "win32",
                    reason="need of checkIntegrity.txt file, results differents on Windows")
def test_071_hydro_reservoir_model_enhanced_04(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "071 Hydro Reservoir Model -enhanced-04"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_072_hydro_reservoir_model_enhanced_05(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "072 Hydro Reservoir Model -enhanced-05"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_073_hydro_reservoir_model_enhanced_06(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "073 Hydro Reservoir Model -enhanced-06"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_074_kcg_on_four_areas_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "074 KCG on Four areas -01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.short
def test_075_kcg_on_four_areas_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "075 KCG on Four areas -02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

# hydro tests

@pytest.mark.short
def test_hydro_hydro_initialization_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "hydro initialization 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    # 26th column = average reservoir level %
    reservoir_levels = fetch_hourly_column_values(study_path, area='area', year=1, col_index=26)
    first_january_midnight = 0
    assert(abs(reservoir_levels[first_january_midnight] - 30) < .05)

@pytest.mark.short
def test_hydro_hydro_initialization_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "hydro initialization 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    # 26th column = average reservoir level %
    reservoir_levels = fetch_hourly_column_values(study_path, area='area', year=1, col_index=26)
    first_march_midnight = 1417
    assert(abs(reservoir_levels[first_march_midnight] - 30) < .05)

@pytest.mark.short
def test_hydro_hydro_preference_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "hydro preference 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    # 26th column = average reservoir level %
    reservoir_levels = fetch_hourly_column_values(study_path, area='area', year=1, col_index=26)
    assert(abs(reservoir_levels[-1] - 30.46) < .05)

@pytest.mark.short
def test_hydro_hydro_preference_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "hydro preference 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    # 26th column = average reservoir level %
    reservoir_levels = fetch_hourly_column_values(study_path, area='area', year=1, col_index=26)
    assert(reservoir_levels[-1] < 30)

@pytest.mark.medium
def test_000_free_data_sample(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "000 Free Data Sample"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_017_probabilistic_vs_deterministic_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "017 Probabilistic vs deterministic - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_032_exploited_wind_fields(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "032 Exploited Wind Fields"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_035_mixed_expansion_smart_grid_model_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "035 Mixed Expansion - Smart grid model 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_038_multistage_study_3_ptdf(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "038 Multistage study-3-PTDF"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_039_multistage_study_4_kirchhoff(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "039 Multistage study-4-Kirchhoff"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_041_multistage_study_6_ntc(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "041 Multistage study-6-NTC"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_042_multistage_study_7_ptdf(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "042 Multistage study-7-PTDF"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_043_multistage_study_8_kirchhoff(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "043 Multistage study-8-Kirchhoff"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_048_benchmark_cpu_ram_hdd_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "048 Benchmark CPU RAM HDD - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_076_kcg_on_regional_dataset_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "076 KCG on regional dataset 01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_077_kcg_on_regional_dataset_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "077 KCG on regional dataset 02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.medium
def test_078_kcg_on_regional_dataset_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "medium-tests" / "078 KCG on regional dataset 03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_049_benchmark_cpu_ram_hdd_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "049 Benchmark CPU RAM HDD - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_050_export_mps_file(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "050 Export MPS File"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_051_benchmark_cpu_ram_hdd_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "051 Benchmark CPU RAM HDD - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_052_benchmark_cpu_ram_hdd_4(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "052 Benchmark CPU RAM HDD - 4"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_079_zero_power_balance_type_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "079 Zero  Power Balance - Type 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_080_zero_power_balance_type_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "080 Zero  Power Balance - Type 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_081_zero_power_balance_type_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "081 Zero  Power Balance - Type 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_082_zero_power_balance_type_4(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "082 Zero  Power Balance - Type 4"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_083_zero_energy_balance_daily(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "083 Zero  Energy Balance - Daily"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_084_zero_energy_balance_weekly(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "084 Zero  Energy Balance - Weekly"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

@pytest.mark.long
def test_085_zero_energy_balance_annual(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "long-tests" / "085 Zero  Energy Balance - Annual"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_integrity_first_opt(study_path)
    check_integrity_second_opt(study_path)

