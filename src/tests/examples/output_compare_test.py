from pathlib import Path
import os
import sys
import glob
import shutil

import numpy as np
import subprocess

from study import Study

import pytest


ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests')

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

## TESTS ##
@pytest.mark.short
def test_001_one_node_passive(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_002_thermal_fleet_base(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_003_thermal_fleet_must_run(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "003 Thermal fleet - Must-run"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_004_thermal_fleet_partial_must_run(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "004 Thermal fleet - Partial must-run"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_005_thermal_fleet_minimum_stable_power_and_min_up_down_times(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "005 Thermal fleet - Minimum stable power and min up down times"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_006_thermal_fleet_extra_costs(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "006 Thermal fleet - Extra costs"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_007_thermal_fleet_fast_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "007 Thermal fleet - Fast unit commitment"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_008_thermal_fleet_accurate_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "008 Thermal fleet - Accurate unit commitment"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_009_ts_generation_thermal_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "009 TS generation - Thermal power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_010_ts_generation_wind_speed(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "010 TS generation - Wind speed"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_011_ts_generation_wind_power_small_scale(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "011 TS generation - Wind power - small scale"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_012_ts_generation_wind_power_large_scale(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "012 TS Generation - Wind power - large scale"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_013_ts_generation_solar_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "013 TS Generation - Solar power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_014_ts_generation_load(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "014 TS generation - Load"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_015_ts_generation_hydro_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "015 TS generation - Hydro power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_016_probabilistic_vs_deterministic_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "016 Probabilistic vs deterministic - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_018_probabilistic_vs_deterministic_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "018 Probabilistic vs deterministic - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_020_single_mesh_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "020 Single mesh - DC law"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_021_four_areas_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "021 Four areas - DC law"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_022_negative_marginal_price(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "022 Negative marginal price"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_023_anti_pricewise_flows(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "023 Anti-pricewise flows"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_024_hurdle_costs_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "024 Hurdle costs - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_025_hurdle_costs_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "025 Hurdle costs - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_026_day_ahead_reserve_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "026 Day ahead reserve - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_027_day_ahead_reserve_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "027 Day ahead reserve - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_028_pumped_storage_plant_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "028 Pumped storage plant - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_029_pumped_storage_plant_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "029 Pumped storage plant - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_030_pumped_storage_plant_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "030 Pumped storage plant - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_031_wind_analysis(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "031 Wind Analysis"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_033_mixed_expansion_storage(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "033 Mixed Expansion - Storage"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_034_mixed_expansion_smart_grid_model_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "034 Mixed Expansion - Smart grid model 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_036_multistage_study_1_isolated_systems(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "036 Multistage study -1-Isolated systems"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_037_multistage_study_2_copperplate(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "037 Multistage study-2-Copperplate"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_040_multistage_study_5_derated(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "040 Multistage study-5-Derated"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_044_psp_strategies_1_no_psp(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "044 PSP strategies-1-No PSP"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_045_psp_strategies_2_det_pumping(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "045 PSP strategies-2-Det pumping"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_046_psp_strategies_3_opt_daily(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "046 PSP strategies-3-Opt daily"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_047_psp_strategies_4_opt_weekly(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "047 PSP strategies-4-Opt weekly"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_053_system_map_editor_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "053 System Map Editor - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_054_system_map_editor_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "054 System Map Editor -2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_055_system_map_editor_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "055 System Map Editor - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_056_system_map_editor_4(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "056 System Map Editor - 4"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_057_four_areas_grid_outages_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "057 Four areas - Grid outages  01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_058_four_areas_grid_outages_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "058 Four areas - Grid outages  02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_059_four_areas_grid_outages_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "059 Four areas - Grid outages  03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_060_four_areas_grid_outages_04(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "060 Four areas - Grid outages  04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_061_four_areas_grid_outages_05(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "061 Four areas - Grid outages  05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_062_grid_topology_changes_on_contingencies_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "062 Grid-Topology changes on contingencies 01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_063_grid_topology_changes_on_contingencies_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "063 Grid-Topology changes on contingencies 02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_064_probabilistic_exchange_capacity(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "064 Probabilistic exchange capacity"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_065_pumped_storage_plant_explicit_model_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "065 Pumped storage plant -explicit model-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_066_pumped_storage_plant_explicit_model_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "066 Pumped storage plant -explicit model-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_067_pumped_storage_plant_explicit_model_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "067 Pumped storage plant -explicit model-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_068_hydro_reservoir_model_enhanced_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "068 Hydro Reservoir Model -enhanced-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_069_hydro_reservoir_model_enhanced_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "069 Hydro Reservoir Model -enhanced-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_070_hydro_reservoir_model_enhanced_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "070 Hydro Reservoir Model -enhanced-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_071_hydro_reservoir_model_enhanced_04(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "071 Hydro Reservoir Model -enhanced-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_072_hydro_reservoir_model_enhanced_05(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "072 Hydro Reservoir Model -enhanced-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_073_hydro_reservoir_model_enhanced_06(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "073 Hydro Reservoir Model -enhanced-06"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_074_kcg_on_four_areas_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "074 KCG on Four areas -01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_075_kcg_on_four_areas_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "075 KCG on Four areas -02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)
    
@pytest.mark.short
def test_playlist_0(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-0"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
def test_playlist_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)
    
@pytest.mark.short
def test_playlist_12(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-12"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)
    
@pytest.mark.short
def test_playlist_13(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-13"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)
    
@pytest.mark.short
def test_playlist_23(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-23"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_playlist_psp_misc_ndg(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-psp-misc-ndg"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)
    
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_06(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_06"
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
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_07"
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
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_08"
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
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_09"
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
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_10"
    enable_study_output(study_path, True)    
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path)
