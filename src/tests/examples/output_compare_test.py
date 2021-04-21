from pathlib import Path
import os
import sys
import glob
import shutil

import numpy as np
import subprocess

from study import Study

import pytest


EXAMPLE_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests')
NONREG_STUDIES_PATH = Path('../resources/Antares_Simulator_NonReg')

RTOL_OVERRIDE_LINUX = {"CO2 EMIS." : 1e-3, "FLOW LIN." : 1e-3 , "UCAP LIN." : 1e-3, "H. INFL" : 1e-3 , "H. STOR" : 1e-3 , "H. OVFL" : 1e-3 , "OV. COST" : 1e-3 , "LIGNITE" : 1e-3 , "CONG. FEE (ABS.)" : 1e-3 , "sb" : 1e-3 , "MARG. COST" : 1e-3 , "DTG MRG" : 1e-3 , "BALANCE" : 1e-3 , "BASE" : 1e-3 , "MRG. PRICE" : 1e-3 , "OP. COST" : 1e-3 , "SEMI BASE" : 1e-3 ,"COAL" : 1e-3 , "MAX MRG" : 1e-3 , "UNSP. ENRG" : 1e-3}
ATOL_OVERRIDE_LINUX = {"CO2 EMIS." : 1, "CONG. FEE (ALG.)" : 1, "FLOW LIN." : 1, "UCAP LIN." : 1, "peak" : 1, "PEAK" : 1, "H. INFL" : 1, "H. STOR" : 1, "HURDLE COST" : 1, "H. OVFL" : 1 , "LOAD" : 1, "CONG. FEE (ABS.)" : 1 , "sb" : 1 , "MISC. DTG" : 1 , "DTG MRG" : 1 , "BALANCE" : 1 , "BASE" : 1 , "OP. COST" : 1 , "SEMI BASE" : 1 , "COAL" : 1 , "p" : 1 , "MAX MRG" : 1 , "UNSP. ENRG" : 1 , "SOLAR" : 1 , "b" : 1 , "NODU" : 1 , "H. ROR" : 1}

def searching_all_directories(directory):
    dir_path = Path(directory)
    assert(dir_path.is_dir())
    dir_list = []
    for x in dir_path.iterdir():
        if x.is_dir():
            dir_list.append(x)
    return dir_list

def find_output_result_dir(output_dir):
    list_output_dir = searching_all_directories(output_dir)
    assert len(list_output_dir) == 1

    list_dir = searching_all_directories(list_output_dir[0])

    dir_list = []
    for x in list_dir:
        dir_path = Path(x)
        if dir_path.is_dir() and (dir_path.name == "adequacy" or dir_path.name == "economy" or dir_path.name == "adequacy-draft"):
            dir_list.append(x)
    assert len(dir_list) == 1
    return dir_list[0]

def get_header_values(values_path : Path) -> np.array :
    max_row = 3
    # skip_header=4 in order to skip the unused part header of the file
    # max_rows=3 to select 3 headers row (name, unit, type : std, min, max, exp,...)
    output_values = np.genfromtxt(values_path, delimiter='\t', skip_header=4, max_rows=max_row, dtype=str)
    return output_values

def get_output_values(values_path : Path) -> np.array :
    max_row = 8760
    # skip_header=7 in order to skip the header of the file
    # max_rows=8760 to select all year
    output_values = np.genfromtxt(values_path, delimiter='\t', skip_header=7, max_rows=max_row)
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

def generate_reference_values(solver_path, path, use_ortools, ortools_solver):

    enable_study_output(path,True)

    reference_path = path / 'reference'
    os.makedirs(reference_path, exist_ok=True)
    launch_solver(solver_path,path, use_ortools, ortools_solver)

    output_path = path / 'output'

    result_dir = find_output_result_dir(output_path)
    shutil.copytree(result_dir, reference_path / 'output' / result_dir.name)

def run_study(solver_path, path, use_ortools, ortools_solver):
    # Launch antares-solver
    launch_solver(solver_path, path, use_ortools, ortools_solver)

def enable_study_output(study_path, enable):
    st = Study(str(study_path))
    st.check_files_existence()

    synthesis_value = "true" if enable else "false"
    st.set_variable(variable = "synthesis", value = synthesis_value, file_nick_name="general")

def compare_directory(result_dir, reference_dir):
    assert (result_dir.is_dir())
    assert (reference_dir.is_dir())

    for x in result_dir.iterdir():
        if x.is_dir():
            if x.name != 'grid':
                compare_directory(x, reference_dir / x.name)
        else:

            uncompared_file_name = ['id-daily.txt', 'id-hourly.txt']

            if not x.name in uncompared_file_name:
                reference_headers = get_header_values(reference_dir / x.name)
                reference_values = get_output_values(reference_dir / x.name)

                output_headers = get_header_values(x)
                output_values = get_output_values(x)

                np.testing.assert_equal(reference_headers,output_headers, err_msg="headers dismatch in " + str(reference_dir / x.name), verbose=True)


                for i in range(len(output_headers[0])):
                    col_name=output_headers[0,i]
                    err_msg = "values dismatch in '" + str(reference_dir / x.name) + "' for '" + col_name + "' column"
                    rtol=1e-4
                    atol=0

                    if sys.platform=="linux":
                        if col_name in RTOL_OVERRIDE_LINUX:
                            rtol = RTOL_OVERRIDE_LINUX[col_name]
                        if col_name in ATOL_OVERRIDE_LINUX:
                            atol = ATOL_OVERRIDE_LINUX[col_name]

                    if reference_values.ndim > 1:
                        np.testing.assert_allclose(reference_values[:, i], output_values[:, i], rtol=rtol, atol=atol, equal_nan=True, err_msg=err_msg, verbose=True)
                    else:
                        np.testing.assert_allclose(reference_values[i], output_values[i], rtol=rtol, atol=atol, equal_nan=True, err_msg=err_msg, verbose=True)


def check_output_values(study_path):
    result_dir = find_output_result_dir(study_path / 'output')
    reference_dir = find_output_result_dir(study_path / 'reference')
    compare_directory(result_dir, reference_dir)
    remove_outputs(study_path)

## EXAMPLES ##
@pytest.mark.short
def test_001_one_node_passive(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "001 One node - passive"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_002_thermal_fleet_base(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_003_thermal_fleet_must_run(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "003 Thermal fleet - Must-run"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_004_thermal_fleet_partial_must_run(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "004 Thermal fleet - Partial must-run"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_005_thermal_fleet_minimum_stable_power_and_min_up_down_times(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "005 Thermal fleet - Minimum stable power and min up down times"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_006_thermal_fleet_extra_costs(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "006 Thermal fleet - Extra costs"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_007_thermal_fleet_fast_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "007 Thermal fleet - Fast unit commitment"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_008_thermal_fleet_accurate_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "008 Thermal fleet - Accurate unit commitment"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_009_ts_generation_thermal_power(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "009 TS generation - Thermal power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_010_ts_generation_wind_speed(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "010 TS generation - Wind speed"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_011_ts_generation_wind_power_small_scale(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "011 TS generation - Wind power - small scale"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_012_ts_generation_wind_power_large_scale(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "012 TS Generation - Wind power - large scale"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_013_ts_generation_solar_power(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "013 TS Generation - Solar power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_014_ts_generation_load(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "014 TS generation - Load"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_015_ts_generation_hydro_power(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "015 TS generation - Hydro power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_016_probabilistic_vs_deterministic_1(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "016 Probabilistic vs deterministic - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_018_probabilistic_vs_deterministic_3(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "018 Probabilistic vs deterministic - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_020_single_mesh_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "020 Single mesh - DC law"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_021_four_areas_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "021 Four areas - DC law"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_022_negative_marginal_price(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "022 Negative marginal price"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_023_anti_pricewise_flows(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "023 Anti-pricewise flows"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_024_hurdle_costs_1(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "024 Hurdle costs - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_025_hurdle_costs_2(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "025 Hurdle costs - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_026_day_ahead_reserve_1(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "026 Day ahead reserve - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_027_day_ahead_reserve_2(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "027 Day ahead reserve - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_028_pumped_storage_plant_1(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "028 Pumped storage plant - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_029_pumped_storage_plant_2(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "029 Pumped storage plant - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_030_pumped_storage_plant_3(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "030 Pumped storage plant - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_031_wind_analysis(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "031 Wind Analysis"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_033_mixed_expansion_storage(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "033 Mixed Expansion - Storage"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_034_mixed_expansion_smart_grid_model_1(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "034 Mixed Expansion - Smart grid model 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_036_multistage_study_1_isolated_systems(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "036 Multistage study -1-Isolated systems"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_037_multistage_study_2_copperplate(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "037 Multistage study-2-Copperplate"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_040_multistage_study_5_derated(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "040 Multistage study-5-Derated"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_044_psp_strategies_1_no_psp(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "044 PSP strategies-1-No PSP"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_045_psp_strategies_2_det_pumping(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "045 PSP strategies-2-Det pumping"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_046_psp_strategies_3_opt_daily(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "046 PSP strategies-3-Opt daily"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_047_psp_strategies_4_opt_weekly(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "047 PSP strategies-4-Opt weekly"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_053_system_map_editor_1(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "053 System Map Editor - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_054_system_map_editor_2(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "054 System Map Editor -2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_055_system_map_editor_3(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "055 System Map Editor - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_056_system_map_editor_4(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "056 System Map Editor - 4"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_057_four_areas_grid_outages_01(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "057 Four areas - Grid outages  01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_058_four_areas_grid_outages_02(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "058 Four areas - Grid outages  02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_059_four_areas_grid_outages_03(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "059 Four areas - Grid outages  03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_060_four_areas_grid_outages_04(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "060 Four areas - Grid outages  04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_061_four_areas_grid_outages_05(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "061 Four areas - Grid outages  05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_062_grid_topology_changes_on_contingencies_01(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "062 Grid-Topology changes on contingencies 01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_063_grid_topology_changes_on_contingencies_02(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "063 Grid-Topology changes on contingencies 02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_064_probabilistic_exchange_capacity(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "064 Probabilistic exchange capacity"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_065_pumped_storage_plant_explicit_model_01(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "065 Pumped storage plant -explicit model-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_066_pumped_storage_plant_explicit_model_02(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "066 Pumped storage plant -explicit model-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_067_pumped_storage_plant_explicit_model_03(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "067 Pumped storage plant -explicit model-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_068_hydro_reservoir_model_enhanced_01(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "068 Hydro Reservoir Model -enhanced-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_069_hydro_reservoir_model_enhanced_02(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "069 Hydro Reservoir Model -enhanced-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_070_hydro_reservoir_model_enhanced_03(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "070 Hydro Reservoir Model -enhanced-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_071_hydro_reservoir_model_enhanced_04(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "071 Hydro Reservoir Model -enhanced-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_072_hydro_reservoir_model_enhanced_05(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "072 Hydro Reservoir Model -enhanced-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_073_hydro_reservoir_model_enhanced_06(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "073 Hydro Reservoir Model -enhanced-06"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_074_kcg_on_four_areas_01(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "074 KCG on Four areas -01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_075_kcg_on_four_areas_02(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "075 KCG on Four areas -02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_0(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-0"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_1(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_2(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_3(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_12(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-12"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_13(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-13"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_23(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-23"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_playlist_psp_misc_ndg(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "short-tests" / "playlist-psp-misc-ndg"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_06(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_06"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_07(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_07"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_08(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_08"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_09(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_09"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_10(use_ortools, ortools_solver, solver_path):
    study_path = EXAMPLE_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_10"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

# NON-REGRESSION
@pytest.mark.non_regression
@pytest.mark.valid_draft
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_052benchmarkcpuramhdd_4_draft(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-draft" / "052 Benchmark CPU RAM HDD - 4_draft"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_adq
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_052benchmarkcpuramhdd_4_adq(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-adq" / "052 Benchmark CPU RAM HDD - 4_adq"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_linkvars(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--linkVars"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_selvars_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_selvars_4(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-4"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_with_geo_trim_no_vars_by_plant(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "with-geo-trim--no-vars-by-plant"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_only_row_bal(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--only-row-bal"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_full_selvars(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--full-selVars"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_selvars_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_adq_selvars_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim-adq-selVars-2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_with_geo_trim_selvars_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "with-geo-trim--selVars-2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_no_vars_by_plant(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--no-vars-by-plant"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_areavars(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--areaVars"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_draft_no_selvar(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "draft--no-selVar"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_adq_selvars_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--adq-selVars-1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_no_row_bal(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--no-row-bal"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_novar(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--noVar"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_draft_hydrobind_no_selvar(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "draft--hydrobind-no-selVar"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_selvars_3(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_with_geo_trim_selvars_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "with-geo-trim--selVars-1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_v601_def_negative_positive_zoneaa(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneAA"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_v601_def_negative_positive_zonecc(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneCC"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_v601_def_negative_positive_zoneb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneB"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_v601_def_negative_positive_zonebb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneBB"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_defaillance_positive_majore(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "defaillance_positive_majore"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_v601_def_negative_positive_zonea(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneA"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_v601_def_negative_positive_zonec(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneC"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-04"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_07(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-07"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_08(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-08"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_09(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-09"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_08_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-08-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_06(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-06"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_06_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-06-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_00_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-00-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_11_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-11-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_09_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-09-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_11(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-11"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_04_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-04-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_10(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-10"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_01_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-01-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-05"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_07_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-07-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_00(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-00"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_03_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-03-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_10_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-10-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_05_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-05-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_02_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-02-gen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_margincost
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_marginal_cost_interco_kirchhoff_ok(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-marginCost" / "marginal_cost_interco-kirchhoff-ok"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_margincost
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_marginalcostrevised(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-marginCost" / "marginal cost revised"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_margincost
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_marginal_cost_interco_freeflow_ok(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-marginCost" / "marginal_cost_interco_freeflow_ok"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_complex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_e2050_5y_6w_fast_mps(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-complex" / "E2050-5y-6w-fast-mps"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_complex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_e2050_5y_6w_accurate_mps(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-complex" / "E2050-5y-6w-accurate-mps"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_complex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrau_frch(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-complex" / "hydrau_FRCH"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_generic_someoff_somemust_tsrm(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - some MUST - TSRM"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_generic_someoff_nooffmust_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - no OFFMUST - TSGEN"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_generic_someoff_somemust_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - some MUST - TSGEN"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_generic_allon_somemust_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - all ON - some MUST - TSGEN"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_generic_allon_somemust_tsrm(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - all ON - some MUST - TSRM"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_generic_someoff_nooffmust_tsrm(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - no OFFMUST - TSRM"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_17(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-17"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_08(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-08"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_50(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-50"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_23(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-23"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_57(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-57"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_38(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-38"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_14(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-14"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_09(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-09"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_36(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-36"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_48(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-48"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_47(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-47"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_26(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-26"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_53(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-53"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_20(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-20"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_55(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-55"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_15(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-15"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_31(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-31"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_12(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-12"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_58(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-58"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_21(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-21"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_49(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-49"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-04"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_25(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-25"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_44(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-44"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_35(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-35"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_45(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-45"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_54(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-54"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_11(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-11"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_27(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-27"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_46(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-46"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_32(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-32"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_10(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-10"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_59(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-59"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_33(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-33"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_51(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-51"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_19(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-19"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_41(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-41"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_52(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-52"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_07(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-07"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_16(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-16"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_06(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-06"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_43(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-43"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_39(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-39"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_29(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-29"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_37(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-37"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_42(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-42"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-05"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_22(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-22"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_34(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-34"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_40(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-40"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_30(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-30"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_18(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-18"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_13(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-13"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_28(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-28"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_00(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-00"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_56(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-56"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_018probabilisticvsdeterministic_3(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "018 Probabilistic vs deterministic - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_001onenode_passive(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "001 One node - passive"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_068hydroreservoirmodel_enhanced_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "068 Hydro Reservoir Model -enhanced-01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_085zeroenergybalance_annual(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "085 Zero  Energy Balance - Annual"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_027dayaheadreserve_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "027 Day ahead reserve - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_052benchmarkcpuramhdd_4(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "052 Benchmark CPU RAM HDD - 4"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_022negativemarginalprice(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "022 Negative marginal price"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_025hurdlecosts_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "025 Hurdle costs - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_037multistagestudy_2_copperplate(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "037 Multistage study-2-Copperplate"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_030pumpedstorageplant_3(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "030 Pumped storage plant - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_031windanalysis(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "031 Wind Analysis"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_073hydroreservoirmodel_enhanced_06(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "073 Hydro Reservoir Model -enhanced-06"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_033mixedexpansion_storage(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "033 Mixed Expansion - Storage"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_069hydroreservoirmodel_enhanced_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "069 Hydro Reservoir Model -enhanced-02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_054systemmapeditor_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "054 System Map Editor -2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_049benchmarkcpuramhdd_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "049 Benchmark CPU RAM HDD - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_019reservoirhydroenergypolicy(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "019  Reservoir hydro energy policy"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_013tsgeneration_solarpower(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "013 TS Generation - Solar power"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_005thermalfleet_minimumstablepowerandminupdowntimes(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "005 Thermal fleet - Minimum stable power and min up down times"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_071hydroreservoirmodel_enhanced_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "071 Hydro Reservoir Model -enhanced-04"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_045pspstrategies_2_detpumping(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "045 PSP strategies-2-Det pumping"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_004thermalfleet_partialmust_run(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "004 Thermal fleet - Partial must-run"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_007thermalfleet_fastunitcommitment(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "007 Thermal fleet - Fast unit commitment"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_072hydroreservoirmodel_enhanced_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "072 Hydro Reservoir Model -enhanced-05"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_006thermalfleet_extracosts(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "006 Thermal fleet - Extra costs"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_039multistagestudy_4_kirchhoff(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "039 Multistage study-4-Kirchhoff"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_050exportmpsfile(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "050 Export MPS File"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_084zeroenergybalance_weekly(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "084 Zero  Energy Balance - Weekly"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_000freedatasample(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "000 Free Data Sample"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_058fourareas_gridoutages02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "058 Four areas - Grid outages  02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_002thermalfleet_base(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "002 Thermal fleet - Base"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_016probabilisticvsdeterministic_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "016 Probabilistic vs deterministic - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_009tsgeneration_thermalpower(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "009 TS generation - Thermal power"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_035mixedexpansion_smartgridmodel2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "035 Mixed Expansion - Smart grid model 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_082zeropowerbalance_type4(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "082 Zero  Power Balance - Type 4"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_024hurdlecosts_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "024 Hurdle costs - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_011tsgeneration_windpower_smallscale(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "011 TS generation - Wind power - small scale"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_044pspstrategies_1_nopsp(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "044 PSP strategies-1-No PSP"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_015tsgeneration_hydropower(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "015 TS generation - Hydro power"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_078kcgonregionaldataset03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "078 KCG on regional dataset 03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_043multistagestudy_8_kirchhoff(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "043 Multistage study-8-Kirchhoff"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_042multistagestudy_7_ptdf(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "042 Multistage study-7-PTDF"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_003thermalfleet_must_run(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "003 Thermal fleet - Must-run"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_063grid_topologychangesoncontingencies02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "063 Grid-Topology changes on contingencies 02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_028pumpedstorageplant_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "028 Pumped storage plant - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_077kcgonregionaldataset02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "077 KCG on regional dataset 02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_074kcgonfourareas_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "074 KCG on Four areas -01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_053systemmapeditor_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "053 System Map Editor - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_047pspstrategies_4_optweekly(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "047 PSP strategies-4-Opt weekly"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_060fourareas_gridoutages04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "060 Four areas - Grid outages  04"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_026dayaheadreserve_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "026 Day ahead reserve - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_038multistagestudy_3_ptdf(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "038 Multistage study-3-PTDF"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_036multistagestudy_1_isolatedsystems(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "036 Multistage study -1-Isolated systems"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_029pumpedstorageplant_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "029 Pumped storage plant - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_079zeropowerbalance_type1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "079 Zero  Power Balance - Type 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_023anti_pricewiseflows(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "023 Anti-pricewise flows"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_064probabilisticexchangecapacity(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "064 Probabilistic exchange capacity"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_017probabilisticvsdeterministic_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "017 Probabilistic vs deterministic - 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_062grid_topologychangesoncontingencies01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "062 Grid-Topology changes on contingencies 01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_008thermalfleet_accurateunitcommitment(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "008 Thermal fleet - Accurate unit commitment"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_056systemmapeditor_4(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "056 System Map Editor - 4"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_041multistagestudy_6_ntc(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "041 Multistage study-6-NTC"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_083zeroenergybalance_daily(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "083 Zero  Energy Balance - Daily"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_010tsgeneration_windspeed(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "010 TS generation - Wind speed"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_061fourareas_gridoutages05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "061 Four areas - Grid outages  05"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_080zeropowerbalance_type2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "080 Zero  Power Balance - Type 2"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_048benchmarkcpuramhdd_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "048 Benchmark CPU RAM HDD - 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_046pspstrategies_3_optdaily(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "046 PSP strategies-3-Opt daily"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_067pumpedstorageplant_explicitmodel_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "067 Pumped storage plant -explicit model-03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_034mixedexpansion_smartgridmodel1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "034 Mixed Expansion - Smart grid model 1"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_021fourareas_dclaw(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "021 Four areas - DC law"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_066pumpedstorageplant_explicitmodel_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "066 Pumped storage plant -explicit model-02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_075kcgonfourareas_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "075 KCG on Four areas -02"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_032exploitedwindfields(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "032 Exploited Wind Fields"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_065pumpedstorageplant_explicitmodel_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "065 Pumped storage plant -explicit model-01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_040multistagestudy_5_derated(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "040 Multistage study-5-Derated"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_070hydroreservoirmodel_enhanced_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "070 Hydro Reservoir Model -enhanced-03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_012tsgeneration_windpower_largescale(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "012 TS Generation - Wind power - large scale"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_057fourareas_gridoutages01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "057 Four areas - Grid outages  01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_020singlemesh_dclaw(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "020 Single mesh - DC law"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_059fourareas_gridoutages03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "059 Four areas - Grid outages  03"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_076kcgonregionaldataset01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "076 KCG on regional dataset 01"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_051benchmarkcpuramhdd_3(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "051 Benchmark CPU RAM HDD - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_055systemmapeditor_3(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "055 System Map Editor - 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_014tsgeneration_load(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "014 TS generation - Load"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ex
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_081zeropowerbalance_type3(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ex" / "081 Zero  Power Balance - Type 3"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ts_gen_export
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_tsgeneration_and_export_1000(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ts-gen-export" / "TS generation-and-export-1000"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_ts_gen_export
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_tsgeneration_and_export_100(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ts-gen-export" / "TS generation-and-export-100"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_00_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-00_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_03_random_nbrs_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-03-random_nbrs_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallelisation_complexe_pl_2on12y_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallelisation-complexe-pl-2on12Y_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_loadsave_2node_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-2node_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_paral_cplx_pl_7on12_scbuild_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-paral-cplx-pl-7on12-scBuild_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fourareas_dclaw_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Four areas - DC law_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_00_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-00_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_nomustrun_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02-noMustRun_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_districts_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-districts_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallelisation_complexe_pl_2on12y_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallelisation-complexe-pl-2on12Y_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_01_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-01_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_paral_cplx_pl_7on12_scbuild_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-paral-cplx-pl-7on12-scBuild_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_loadsave_2node_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-2node_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_01_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-01_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_multistagestudy_3_ptdf_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Multistage study-3-PTDF_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fourareas_dclaw_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Four areas - DC law_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_loadsave_1node_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-1node_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_loadsave_1node_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-1node_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_multistagestudy_3_ptdf_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Multistage study-3-PTDF_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_03_random_nbrs_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-03-random_nbrs_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_nomustrun_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02-noMustRun_eco-acc-168"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_districts_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-districts_eco-fast-24"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxday_freemod(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxday-freemod"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_adequacy(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-Adequacy"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_scenario_builder_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-scenario_builder-tsgen"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_scenario_builder_readymade(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-scenario_builder-readymade"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxweek_freemod(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxweek-freemod"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxweek_minexc(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxweek-minexc"
                     run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_draft(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-draft"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxday_minexc(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxday-minexc"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

