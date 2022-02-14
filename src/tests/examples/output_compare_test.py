import pytest
from pathlib import Path
import sys

from antares_test_utils import *

## TESTS ##
@pytest.mark.short
def test_001_one_node_passive(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_002_thermal_fleet_base(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_003_thermal_fleet_must_run(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "003 Thermal fleet - Must-run"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_004_thermal_fleet_partial_must_run(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "004 Thermal fleet - Partial must-run"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_005_thermal_fleet_minimum_stable_power_and_min_up_down_times(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "005 Thermal fleet - Minimum stable power and min up down times"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_006_thermal_fleet_extra_costs(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "006 Thermal fleet - Extra costs"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_007_thermal_fleet_fast_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "007 Thermal fleet - Fast unit commitment"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_008_thermal_fleet_accurate_unit_commitment(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "008 Thermal fleet - Accurate unit commitment"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_009_ts_generation_thermal_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "009 TS generation - Thermal power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_010_ts_generation_wind_speed(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "010 TS generation - Wind speed"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_011_ts_generation_wind_power_small_scale(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "011 TS generation - Wind power - small scale"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_012_ts_generation_wind_power_large_scale(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "012 TS Generation - Wind power - large scale"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_013_ts_generation_solar_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "013 TS Generation - Solar power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_014_ts_generation_load(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "014 TS generation - Load"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_015_ts_generation_hydro_power(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "015 TS generation - Hydro power"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_016_probabilistic_vs_deterministic_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "016 Probabilistic vs deterministic - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_018_probabilistic_vs_deterministic_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "018 Probabilistic vs deterministic - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_020_single_mesh_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "020 Single mesh - DC law"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_021_four_areas_dc_law(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "021 Four areas - DC law"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_022_negative_marginal_price(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "022 Negative marginal price"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_023_anti_pricewise_flows(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "023 Anti-pricewise flows"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_024_hurdle_costs_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "024 Hurdle costs - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_025_hurdle_costs_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "025 Hurdle costs - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_026_day_ahead_reserve_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "026 Day ahead reserve - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_027_day_ahead_reserve_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "027 Day ahead reserve - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_028_pumped_storage_plant_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "028 Pumped storage plant - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_029_pumped_storage_plant_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "029 Pumped storage plant - 2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_030_pumped_storage_plant_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "030 Pumped storage plant - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_031_wind_analysis(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "031 Wind Analysis"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_033_mixed_expansion_storage(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "033 Mixed Expansion - Storage"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_034_mixed_expansion_smart_grid_model_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "034 Mixed Expansion - Smart grid model 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_036_multistage_study_1_isolated_systems(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "036 Multistage study -1-Isolated systems"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_037_multistage_study_2_copperplate(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "037 Multistage study-2-Copperplate"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_040_multistage_study_5_derated(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "040 Multistage study-5-Derated"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_044_psp_strategies_1_no_psp(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "044 PSP strategies-1-No PSP"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_045_psp_strategies_2_det_pumping(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "045 PSP strategies-2-Det pumping"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_046_psp_strategies_3_opt_daily(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "046 PSP strategies-3-Opt daily"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_047_psp_strategies_4_opt_weekly(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "047 PSP strategies-4-Opt weekly"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_053_system_map_editor_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "053 System Map Editor - 1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_054_system_map_editor_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "054 System Map Editor -2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_055_system_map_editor_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "055 System Map Editor - 3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_056_system_map_editor_4(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "056 System Map Editor - 4"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_057_four_areas_grid_outages_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "057 Four areas - Grid outages  01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_058_four_areas_grid_outages_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "058 Four areas - Grid outages  02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_059_four_areas_grid_outages_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "059 Four areas - Grid outages  03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_060_four_areas_grid_outages_04(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "060 Four areas - Grid outages  04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_061_four_areas_grid_outages_05(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "061 Four areas - Grid outages  05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_062_grid_topology_changes_on_contingencies_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "062 Grid-Topology changes on contingencies 01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_063_grid_topology_changes_on_contingencies_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "063 Grid-Topology changes on contingencies 02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_064_probabilistic_exchange_capacity(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "064 Probabilistic exchange capacity"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())


@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_065_pumped_storage_plant_explicit_model_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "065 Pumped storage plant -explicit model-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)

    var_names = ['AVL DTG', 'b', 'BALANCE', 'CO2 EMIS.', 'COAL', 'DTG MRG', 'FLOW LIN.', 'GAS', 'H. INFL']
    var_names.extend(['H. PUMP', 'H. ROR', 'H. STOR', 'LOAD', 'MAX MRG', 'NODU', 'OP. COST', 'p', 'sb'])
    var_names.extend(['SPIL. ENRG', 'UCAP LIN.', 'WIND'])

    tolerances = get_tolerances()
    for var_name in var_names:
        tolerances.set_absolute(var_name, 1)

    check_output_values(study_path, tolerances)


@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_066_pumped_storage_plant_explicit_model_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "066 Pumped storage plant -explicit model-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)

    var_names = ['AVL DTG', 'b', 'BALANCE', 'CO2 EMIS.', 'COAL', 'DTG MRG', 'FLOW LIN.', 'GAS', 'H. INFL']
    var_names.extend(['H. PUMP', 'H. ROR', 'H. STOR', 'LOAD', 'MAX MRG', 'NODU', 'OP. COST', 'p', 'sb'])
    var_names.extend(['SPIL. ENRG', 'UCAP LIN.', 'WIND'])

    tolerances = get_tolerances()
    for var_name in var_names:
        tolerances.set_absolute(var_name, 1)

    check_output_values(study_path, tolerances)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_067_pumped_storage_plant_explicit_model_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "067 Pumped storage plant -explicit model-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)

    var_names = ['AVL DTG', 'b', 'BALANCE', 'CO2 EMIS.', 'COAL', 'DTG MRG', 'FLOW LIN.', 'GAS', 'H. INFL']
    var_names.extend(['H. OVFL', 'H. ROR', 'LOAD', 'MAX MRG', 'NODU', 'OIL', 'OP. COST', 'p', 'sb'])
    var_names.extend(['SPIL. ENRG', 'UCAP LIN.', 'UNSP. ENRG', 'WIND'])

    tolerances = get_tolerances()
    for var_name in var_names:
        tolerances.set_absolute(var_name, 1)

    check_output_values(study_path, tolerances)

@pytest.mark.short
def test_068_hydro_reservoir_model_enhanced_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "068 Hydro Reservoir Model -enhanced-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_069_hydro_reservoir_model_enhanced_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "069 Hydro Reservoir Model -enhanced-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_070_hydro_reservoir_model_enhanced_03(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "070 Hydro Reservoir Model -enhanced-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_071_hydro_reservoir_model_enhanced_04(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "071 Hydro Reservoir Model -enhanced-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_072_hydro_reservoir_model_enhanced_05(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "072 Hydro Reservoir Model -enhanced-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_073_hydro_reservoir_model_enhanced_06(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "073 Hydro Reservoir Model -enhanced-06"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_074_kcg_on_four_areas_01(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "074 KCG on Four areas -01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_075_kcg_on_four_areas_02(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "075 KCG on Four areas -02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_playlist_0(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-0"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_playlist_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_playlist_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_playlist_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_playlist_12(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-12"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_playlist_13(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-13"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_playlist_23(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-23"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skip("TODO restore Windows run for v8.2")
def test_playlist_psp_misc_ndg(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "playlist-psp-misc-ndg"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)

    var_names = ['CCGT_new', 'CCGT-old-2', 'CONG. FEE (ABS.)', 'CONG. FEE (ALG.)', 'gas_ccgt new', 'gas_ccgt old 1']
    var_names.extend(['gas_ccgt old 2', 'gas_conventional old 1', 'gas_ocgt new', 'gas_ocgt old', 'hard coal_new'])
    var_names.extend(['hard coal_old 1', 'HURDLE COST', 'LIGNITE', 'lignite_new', 'Lignite-new', 'lignite_old 1'])
    var_names.extend(['lignite_old 2', 'Lignite-old-1', 'Lignite-old-2', 'NP COST', 'nuclear_nuclear', 'OCGT_new'])
    var_names.extend(['OCGT-old', 'SOLAR'])

    tolerances = get_tolerances()
    for var_name in var_names:
        tolerances.set_absolute(var_name, 1)

    check_output_values(study_path, tolerances)

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_06(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_06"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    send_std_error_to = subprocess.DEVNULL
    run_study(solver_path, study_path, use_ortools, ortools_solver, send_std_error_to)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_07(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_07"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    send_std_error_to = subprocess.DEVNULL
    run_study(solver_path, study_path, use_ortools, ortools_solver, send_std_error_to)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_08(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_08"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    send_std_error_to = subprocess.DEVNULL
    run_study(solver_path, study_path, use_ortools, ortools_solver, send_std_error_to)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_09(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_09"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    send_std_error_to = subprocess.DEVNULL
    run_study(solver_path, study_path, use_ortools, ortools_solver, send_std_error_to)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_unfeasible_problem_10(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" /"unfeasible_problem_10"
    enable_study_output(study_path, True)
    st = Study(str(study_path))
    st.set_variable(variable="include-unfeasible-problem-behavior", value="warning-dry", file_nick_name="general")
    send_std_error_to = subprocess.DEVNULL
    run_study(solver_path, study_path, use_ortools, ortools_solver, send_std_error_to)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_renewables_1(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "renewables-1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_renewables_2(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "renewables-2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_renewables_3(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "renewables-3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    enable_study_output(study_path, False)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_renewables_1_cluster_ts_prod_factor(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "renewable-1-cluster-ts-prod-factor"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_renewables_2_cluster_ts_prod_factor(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "renewable-2-clusters-ts-prod-factor"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_renewables_ts_prod_factor_cluster_disabled(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "renewable-ts-prod-factor-cluster-disabled"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_ts_gen_select_thermal_cluster_readymade(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "TS gen select thermal cluster - ready made"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
def test_ts_gen_select_thermal_cluster_stochastic(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "TS gen select thermal cluster - stochastic"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.short
@pytest.mark.skip("TODO restore Windows run for v8.2")
def test_ts_gen_select_thermal_cluster_refresh_force_gen(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "Thermal refresh/"
    run_study(solver_path, study_path, use_ortools, ortools_solver)

    tolerances = get_tolerances()
    tolerances.set_absolute('cluster', 1)

    check_output_values(study_path, tolerances)

@pytest.mark.short
def test_ntc_sc_builder(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "Multiple NTC sc-builder"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())
    
@pytest.mark.short
def test_ntc_intramodal_two_links_two_timeseries(use_ortools, ortools_solver, solver_path):
    study_path = ALL_STUDIES_PATH / "short-tests" / "NTC intramodal"
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())
