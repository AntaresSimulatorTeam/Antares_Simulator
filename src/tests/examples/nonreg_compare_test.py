import pytest
from pathlib import Path
import sys

from antares_test_utils import *

NONREG_STUDIES_PATH = Path('../resources/NonRegTests')

# NON-REGRESSION
@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
def test_bind_08_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-08-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
def test_bind_09_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-09-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
def test_bind_10_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-10-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.short
def test_v601_def_negative_positive_zonea(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneA"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.short
def test_v601_def_negative_positive_zoneaa(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneAA"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.short
def test_v601_def_negative_positive_zoneb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneB"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.short
def test_v601_def_negative_positive_zonebb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneBB"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.short
def test_v601_def_negative_positive_zonecc(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneCC"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_draft_hydrobind_no_selvar(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "draft--hydrobind-no-selVar"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_00(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-00"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_06(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-06"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_07(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-07"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_08(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-08"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_09(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-09"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_11(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-11"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_15(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-15"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_16(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-16"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_17(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-17"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_20(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-20"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_21(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-21"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_25(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-25"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_41(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-41"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_42(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-42"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_43(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-43"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_47(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-47"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_48(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-48"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_49(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-49"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_50(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-50"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_margincost
@pytest.mark.short
def test_marginal_cost_interco_kirchhoff_ok(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-marginCost" / "marginal_cost_interco-kirchhoff-ok"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_margincost
@pytest.mark.short
def test_marginal_cost_interco_freeflow_ok(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-marginCost" / "marginal_cost_interco_freeflow_ok"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_districts_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-districts_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_loadsave_1node_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-1node_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_loadsave_1node_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-1node_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_loadsave_2node_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-2node_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_loadsave_2node_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-loadsave-2node_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
def test_bind_07_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-07-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
def test_bind_11_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-11-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.short
def test_v601_def_negative_positive_zonec(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "V601-Def-negative-positive-zoneC"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_10(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-10"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_14(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-14"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_18(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-18"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_19(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-19"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_22(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-22"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_23(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-23"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_26(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-26"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_27(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-27"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_28(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-28"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_35(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-35"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_36(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-36"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_37(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-37"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_38(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-38"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_39(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-39"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_40(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-40"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_45(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-45"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_accurate_few_weeks_five_areas_hydro_pricing_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/Few_weeks-five-areas/hydro-pricing-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_accurate_few_weeks_five_areas_hydro_pricing_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/Few_weeks-five-areas/hydro-pricing-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_accurate_few_weeks_five_areas_hydro_pricing_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/Few_weeks-five-areas/hydro-pricing-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_accurate_few_weeks_five_areas_hydro_pricing_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/Few_weeks-five-areas/hydro-pricing-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_accurate_few_weeks_five_areas_hydro_pricing_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/Few_weeks-five-areas/hydro-pricing-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_one_node_differentstyles_hydro_pricing_he(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-one_node_different styles/hydro-pricing-HE"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_one_node_differentstyles_hydro_pricing_m(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-one_node_different styles/hydro-pricing-M"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_fast_few_weeks_five_areas_hydro_pricing_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/Few_weeks-five-areas/hydro-pricing-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_fast_few_weeks_five_areas_hydro_pricing_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/Few_weeks-five-areas/hydro-pricing-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_fast_few_weeks_five_areas_hydro_pricing_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/Few_weeks-five-areas/hydro-pricing-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_fast_few_weeks_five_areas_hydro_pricing_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/Few_weeks-five-areas/hydro-pricing-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
def test_fast_few_weeks_five_areas_hydro_pricing_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/Few_weeks-five-areas/hydro-pricing-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_one_node_differentstyles_hydro_pricing_he(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-one_node_different styles/hydro-pricing-HE"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_one_node_differentstyles_hydro_pricing_m_hb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-one_node_different styles/hydro-pricing-M_hb"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_margincost
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_marginalcostrevised(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-marginCost" / "marginal cost revised"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_fourareas_dclaw_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Four areas - DC law_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_districts_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-districts_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
def test_bind_04_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-04-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
def test_bind_05_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-05-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_06_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-06-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_draft_no_selvar(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "draft--no-selVar"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_adq_selvars_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--adq-selVars-1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_linkvars(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--linkVars"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
def test_no_geo_trim_only_row_bal(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--only-row-bal"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_12(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-12"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_13(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-13"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h700_29(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-29"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_33(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-33"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_46(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-46"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.short
def test_hydrobind_scenario_builder_readymade(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-scenario_builder-readymade"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.short
def test_hydrobind_scenario_builder_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-scenario_builder-tsgen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_one_node_differentstyles_hydro_pricing_e(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-one_node_different styles/hydro-pricing-E"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_one_node_differentstyles_hydro_pricing_e_hb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-one_node_different styles/hydro-pricing-E_hb"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_one_node_differentstyles_hydro_pricing_m_hb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-one_node_different styles/hydro-pricing-M_hb"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_one_node_differentstyles_hydro_pricing_e(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-one_node_different styles/hydro-pricing-E"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_one_node_differentstyles_hydro_pricing_m(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-one_node_different styles/hydro-pricing-M"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_00_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-00_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallel_00_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-00_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.short
def test_generic_allon_somemust_tsrm(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - all ON - some MUST - TSRM"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.short
def test_generic_someoff_nooffmust_tsrm(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - no OFFMUST - TSRM"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.short
def test_generic_someoff_somemust_tsrm(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - some MUST - TSRM"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_ts_gen_export
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_tsgeneration_and_export_10(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-ts-gen-export" / "TS generation-and-export-10"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_defaillance
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_defaillance_positive_majore(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-defaillance" / "defaillance_positive_majore"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_areavars(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--areaVars"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_full_selvars(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--full-selVars"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_no_row_bal(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--no-row-bal"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_no_vars_by_plant(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--no-vars-by-plant"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_novar(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--noVar"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_selvars_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
def test_no_geo_trim_selvars_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
def test_no_geo_trim_selvars_3(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-3"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_selvars_4(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim--selVars-4"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_no_geo_trim_adq_selvars_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "no-geo-trim-adq-selVars-2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_with_geo_trim_no_vars_by_plant(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "with-geo-trim--no-vars-by-plant"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_with_geo_trim_selvars_1(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "with-geo-trim--selVars-1"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_filter_outputs
@pytest.mark.short
def test_with_geo_trim_selvars_2(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-filter-outputs" / "with-geo-trim--selVars-2"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_34(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-34"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_52(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-52"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h701_57(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-57"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h701_58(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-58"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_one_node_differentstyles_hydro_pricing_e_hb(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-one_node_different styles/hydro-pricing-E_hb"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_fourareas_dclaw_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Four areas - DC law_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.short
def test_generic_allon_somemust_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - all ON - some MUST - TSGEN"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.short
def test_generic_someoff_nooffmust_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - no OFFMUST - TSGEN"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_thermal
@pytest.mark.short
def test_generic_someoff_somemust_tsgen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-thermal" / "generic - some  OFF - some MUST - TSGEN"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_30(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-30"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_53(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-53"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h701_54(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-54"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h701_55(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-55"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
def test_h701_56(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-56"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h701_59(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H701-59"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_31(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-31"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_h700_51(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-51"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_paral_cplx_pl_7on12_scbuild_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-paral-cplx-pl-7on12-scBuild_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_test_parallelisation_complexe_pl_2on12y_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallelisation-complexe-pl-2on12Y_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_complex
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrau_frch(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-complex" / "hydrau_FRCH"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
def test_multistagestudy_3_ptdf_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Multistage study-3-PTDF_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_five_areas_hydro_pricing_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-five-areas/hydro-pricing-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_five_areas_hydro_pricing_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-five-areas/hydro-pricing-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_five_areas_hydro_pricing_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-five-areas/hydro-pricing-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_five_areas_hydro_pricing_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-five-areas/hydro-pricing-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_five_areas_hydro_pricing_01(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-five-areas/hydro-pricing-01"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_five_areas_hydro_pricing_02(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-five-areas/hydro-pricing-02"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_five_areas_hydro_pricing_03(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-five-areas/hydro-pricing-03"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_five_areas_hydro_pricing_04(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-five-areas/hydro-pricing-04"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_paral_cplx_pl_7on12_scbuild_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-paral-cplx-pl-7on12-scBuild_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallelisation_complexe_pl_2on12y_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallelisation-complexe-pl-2on12Y_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_accurate_yht_five_areas_hydro_pricing_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Accurate/YHT-five-areas/hydro-pricing-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydropricing
@pytest.mark.short
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_fast_yht_five_areas_hydro_pricing_05(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroPricing" / "Fast/YHT-five-areas/hydro-pricing-05"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.medium
def test_multistagestudy_3_ptdf_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "Multistage study-3-PTDF_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxday_freemod(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxday-freemod"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_adequacy(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-Adequacy"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxday_minexc(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxday-minexc"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.medium
def test_bind_03_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-03-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.medium
def test_h700_44(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-44"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydro
@pytest.mark.medium
def test_h700_32(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydro" / "H700-32"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxweek_freemod(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxweek-freemod"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_draft
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_052benchmarkcpuramhdd_4_draft(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-draft" / "052 Benchmark CPU RAM HDD - 4_draft"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_spxweek_minexc(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-spxweek-minexc"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_hydrobind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_hydrobind_draft(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-hydroBind" / "hydrobind-draft"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_01_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-01_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_nomustrun_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02-noMustRun_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_nomustrun_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02-noMustRun_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_01_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-01_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_test_parallel_02_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-02_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_adq
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_052benchmarkcpuramhdd_4_adq(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-adq" / "052 Benchmark CPU RAM HDD - 4_adq"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_00_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-00-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.medium
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_01_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-01-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.long
def test_test_parallel_03_random_nbrs_eco_fast_24(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-03-random_nbrs_eco-fast-24"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_bind
@pytest.mark.long
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_bind_02_gen(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-bind" / "BIND-02-gen"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_complex
@pytest.mark.long
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_e2050_5y_6w_fast_mps(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-complex" / "E2050-5y-6w-fast-mps"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_parallel
@pytest.mark.long
def test_test_parallel_03_random_nbrs_eco_acc_168(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-parallel" / "test-parallel-03-random_nbrs_eco-acc-168"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

@pytest.mark.non_regression
@pytest.mark.valid_complex
@pytest.mark.long
@pytest.mark.skipif(sys.platform=="linux",
                    reason="Results different between linux and windows.")
def test_e2050_5y_6w_accurate_mps(use_ortools, ortools_solver, solver_path):
    study_path = NONREG_STUDIES_PATH / "valid-complex" / "E2050-5y-6w-accurate-mps"
    enable_study_output(study_path, True)
    run_study(solver_path, study_path, use_ortools, ortools_solver)
    check_output_values(study_path, get_tolerances())

