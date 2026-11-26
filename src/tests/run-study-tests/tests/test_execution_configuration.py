import configparser
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import sys
from unittest.mock import Mock

import pytest

from actions_on_study.create_simulation import create_simulation
from actions_on_study.study_run import RunStudyWithProblemGenerator, RunStudyWithSolver, StudyRun
from retrieve_exe_paths import load_executables
from registered_fixures import check_handler
from parse_studies.load_check_config import load_check_config
from parse_studies.json_collector import contains_json_config, jsonCollector


pytest_plugins = ["pytester"]
SCRIPT_DIR = Path(__file__).resolve().parents[1]


@pytest.fixture
def entry():
    return {
        "name": "example",
        "exe": "antares-solver",
        "options": ["--solver=sirius"],
        "checks": {"simu_table_compare": None},
    }


def write_config(study_path, entries):
    (study_path / "check-config.json").write_text(json.dumps(entries))


def test_collector_preserves_execution_settings(tmp_path, entry, monkeypatch):
    write_config(tmp_path, [entry])
    monkeypatch.chdir(tmp_path)
    collector = jsonCollector([tmp_path])
    collector.collect()
    assert collector.pairs() == [(tmp_path, entry)]
    assert collector.testIds() == [f"example  ({tmp_path.parent.name} / {tmp_path.name})"]


def test_contains_json_config(tmp_path, entry):
    assert not contains_json_config(tmp_path)
    write_config(tmp_path, [entry])
    assert contains_json_config(tmp_path)


def test_collector_skips_missing_config_and_continues(tmp_path, entry, capsys):
    missing = tmp_path / "missing"
    valid = tmp_path / "valid"
    missing.mkdir()
    valid.mkdir()
    write_config(valid, [entry])
    collector = jsonCollector([missing, valid])

    collector.collect()

    assert collector.pairs() == [(valid, entry)]
    assert collector.testIds() == [f"example  ({tmp_path.name} / valid)"]
    assert f"Skipping checks on '{missing}': check-config.json does not exist" in capsys.readouterr().out


def test_collector_reports_other_missing_files(tmp_path, entry, monkeypatch):
    write_config(tmp_path, [entry])
    monkeypatch.setattr("parse_studies.load_check_config.py.SCHEMA_FILE", tmp_path / "missing-schema.json")
    with pytest.raises(FileNotFoundError, match="missing-schema.json"):
        jsonCollector([tmp_path]).collect()


def test_collector_rejects_malformed_config(tmp_path):
    (tmp_path / "check-config.json").write_text("{")
    with pytest.raises(ValueError, match="check-config.json: invalid JSON"):
        jsonCollector([tmp_path]).collect()


@pytest.mark.parametrize(
    "field,value",
    [("exe", None), ("exe", "unknown"), ("options", None), ("options", [42]), ("name", "")],
)
def test_invalid_execution_settings_fail_collection(tmp_path, entry, field, value):
    entry[field] = value
    write_config(tmp_path, [entry])
    with pytest.raises(ValueError, match="check-config.json.*invalid test configuration"):
        jsonCollector([tmp_path]).collect()


@pytest.mark.parametrize("field", ["name", "exe", "options", "checks"])
def test_required_fields(tmp_path, entry, field):
    del entry[field]
    write_config(tmp_path, [entry])
    with pytest.raises(ValueError, match=f"'{field}' is a required property"):
        load_check_config(tmp_path)


def test_empty_options(tmp_path, entry):
    entry["options"] = []
    write_config(tmp_path, [entry])
    assert load_check_config(tmp_path) == [entry]


def test_configuration_errors(tmp_path, entry):
    with pytest.raises(FileNotFoundError, match="check-config.json"):
        load_check_config(tmp_path)
    (tmp_path / "check-config.json").write_text("{")
    with pytest.raises(ValueError, match="check-config.json: invalid JSON"):
        load_check_config(tmp_path)
    write_config(tmp_path, [entry, entry])
    with pytest.raises(ValueError, match="names must be unique"):
        load_check_config(tmp_path)


@pytest.mark.parametrize("runner,study_args", [
    (RunStudyWithSolver, ["-i", "study with spaces"]),
    (RunStudyWithProblemGenerator, ["study with spaces"]),
])
@pytest.mark.parametrize("options", [[], [
    "--named-mps-problems", "--force-parallel=4", "--output=simulation-tables",
    "--simulation-table-stages=optim-nb-1", "--label", "value with spaces",
    "--label", "second value",
]])
def test_runner_command(tmp_path, monkeypatch, capsys, runner, study_args, options):
    process = Mock(returncode=0)
    popen = Mock(return_value=process)
    monkeypatch.setattr(subprocess, "Popen", popen)
    executable = tmp_path / "binary with spaces"
    original_options = list(options)
    supplied_options = list(options)
    simulation = runner(Path("study with spaces"), executable, supplied_options)
    supplied_options.append("--not-part-of-the-run")
    simulation.run()
    popen.assert_called_once_with(
        [str(executable), *study_args, *original_options],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
    )
    process.communicate.assert_called_once_with()
    assert capsys.readouterr().out == f"\nRunning the study with {executable}\n"
    assert isinstance(simulation, StudyRun)
    assert simulation.get_return_code() == 0
    assert simulation.success()


@pytest.mark.parametrize("runner", [RunStudyWithSolver, RunStudyWithProblemGenerator])
def test_expected_and_unexpected_failure(tmp_path, monkeypatch, runner):
    monkeypatch.setattr(subprocess, "Popen", Mock(return_value=Mock(returncode=7)))
    simulation = runner(tmp_path, tmp_path / "binary", [])
    with pytest.raises(AssertionError, match="Solver returned error 7"):
        simulation.run()
    simulation.no_exception_raised_on_failure()
    simulation.run()
    assert simulation.get_return_code() == 7
    assert not simulation.success()


@pytest.mark.parametrize("name,runner", [
    ("antares-solver", RunStudyWithSolver),
    ("antares-problem-generator", RunStudyWithProblemGenerator),
])
def test_simulation_selects_only_requested_binary(tmp_path, entry, name, runner):
    path = tmp_path / name
    path.touch()
    exe_paths = {
        "antares-solver": str(tmp_path / "not-built-solver"),
        "antares-problem-generator": str(tmp_path / "not-built-generator"),
    }
    exe_paths[name] = str(path)
    entry["exe"] = name
    simulation = create_simulation(tmp_path, entry, exe_paths)
    assert isinstance(simulation, runner)
    assert simulation.options == entry["options"]
    path.unlink()
    with pytest.raises(FileNotFoundError, match=f"{name} executable not found"):
        create_simulation(tmp_path, entry, exe_paths)


def test_load_executable_paths_without_interpolation(tmp_path, monkeypatch):
    config = tmp_path / "executables.ini"
    solver = tmp_path / "build with spaces" / "solver%name"
    generator = tmp_path / "generator"
    config.write_text(
        f"[executables]\nantares-solver = {solver}\n"
        f"antares-problem-generator = {generator}\n"
    )
    monkeypatch.chdir(tmp_path.parent)
    assert load_executables(config) == {
        "antares-solver": str(solver),
        "antares-problem-generator": str(generator),
    }


@pytest.mark.parametrize("content,error", [
    ("not an ini file", "invalid executable configuration"),
    ("[other]\n", "missing executable path for antares-solver"),
    ("[executables]\nantares-solver =\n", "missing executable path for antares-solver"),
    ("[executables]\nantares-solver = relative/path\n", "must be absolute"),
])
def test_invalid_executable_paths(tmp_path, content, error):
    config = tmp_path / "executables.ini"
    config.write_text(content)
    with pytest.raises(ValueError, match=error):
        load_executables(config)


def test_missing_executable_configuration(tmp_path):
    with pytest.raises(FileNotFoundError, match="configure CMake"):
        load_executables(tmp_path / "missing.ini")


def test_session_fixture_loads_paths_once(pytester):
    pytester.makeconftest(
        (SCRIPT_DIR / "conftest.py").read_text()
        + """
calls = 0
def load_executables():
    global calls
    calls += 1
    assert calls == 1
    return {"loaded": calls}
"""
    )
    pytester.makepyfile("""
def test_first(exe_paths):
    assert exe_paths == {"loaded": 1}

def test_second(exe_paths):
    assert exe_paths == {"loaded": 1}
""")
    pytester.runpytest("-q").assert_outcomes(passed=2)


def test_generator_skips_are_per_entry(pytester):
    pytester.makeconftest((SCRIPT_DIR / "conftest.py").read_text())
    pytester.makepyfile("""
import pytest

@pytest.mark.parametrize("json_config", [
    {"exe": "antares-solver"},
    {"exe": "antares-problem-generator"},
], ids=["solver", "generator"])
def test_AreaHydroLevel(json_config):
    assert json_config["exe"] == "antares-solver"
""")
    pytester.runpytest("-q").assert_outcomes(passed=1, skipped=1)


@pytest.mark.parametrize("failure_stage", [None, "simulation", "checks"])
def test_check_handler_registers_checks_before_execution(failure_stage):
    events = []
    remover = Mock()
    modifier = Mock()
    simulation = Mock()
    checks = Mock()
    checks.study_modifiers.return_value = [modifier]

    def run(stage):
        events.append(stage)
        if stage == failure_stage:
            raise RuntimeError(stage)

    simulation.run.side_effect = lambda: run("simulation")
    checks.run.side_effect = lambda: run("checks")
    handler = check_handler(remover)
    if failure_stage:
        with pytest.raises(RuntimeError, match=failure_stage):
            handler.run(simulation, checks)
    else:
        handler.run(simulation, checks)
    handler.teardown()
    assert events == (["simulation"] if failure_stage == "simulation" else ["simulation", "checks"])
    modifier.back_to_initial_state.assert_called_once_with()
    remover.run.assert_called_once_with()


def test_fixture_teardown_after_execution_failure(pytester):
    pytester.makepyfile("""
from pathlib import Path
from unittest.mock import Mock

def test_failing(check_runner):
    simulation = Mock()
    simulation.run.side_effect = RuntimeError("expected execution failure")
    modifier = Mock()
    modifier.back_to_initial_state.side_effect = lambda: Path("restored").touch()
    checks = Mock()
    checks.study_modifiers.return_value = [modifier]
    check_runner.run(simulation, checks)

def test_teardown():
    assert Path("restored").exists()
    assert not Path("study/output/generated").exists()
    assert not Path("study/logs").exists()
    assert Path("study/output/reference/keep").exists()
""")
    pytester.makeconftest("""
from pathlib import Path
import pytest

pytest_plugins = ["registered_fixures"]

@pytest.fixture
def study_path():
    path = Path("study")
    (path / "output/generated").mkdir(parents=True)
    (path / "output/reference").mkdir()
    (path / "output/reference/keep").touch()
    (path / "logs").mkdir()
    return path
""")
    pytester.runpytest("-q").assert_outcomes(failed=1, passed=1)


@pytest.mark.parametrize("indirect", [False, True])
def test_shared_fixtures_are_discovered_without_imports(pytester, indirect):
    pytester.makeconftest((SCRIPT_DIR / "conftest.py").read_text())
    study = pytester.path / "study"
    (study / "output" / "reference").mkdir(parents=True)
    (study / "output" / "generated").mkdir()
    (study / "logs").mkdir()
    pytester.makepyfile(f"""
import pytest
from pathlib import Path

@pytest.mark.parametrize("study_path", [Path({str(study)!r})], indirect={indirect})
def test_using_shared_fixtures(study_path, check_runner):
    assert Path(study_path).name == "study"
    assert check_runner is not None
""")
    pytester.runpytest("-q").assert_outcomes(passed=1)
    assert (study / "output" / "reference").exists()
    assert not (study / "output" / "generated").exists()
    assert not (study / "logs").exists()


def test_json_entrypoint_passes_same_simulation(monkeypatch, tmp_path, entry):
    monkeypatch.setattr("parse_studies.look_for_studies.look_for_studies", lambda *args: [])
    spec = importlib.util.spec_from_file_location(
        "json_entrypoint", SCRIPT_DIR / "test_from_json.py"
    )
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    simulation = Mock()
    checks = Mock()
    factory = Mock(return_value=simulation)
    checks_factory = Mock(return_value=checks)
    monkeypatch.setattr(module, "create_simulation", factory)
    monkeypatch.setattr(module, "create_checks", checks_factory)
    check_runner = Mock()
    exe_paths = {}
    module.test(tmp_path, entry, check_runner, exe_paths)
    factory.assert_called_once_with(tmp_path, entry, exe_paths)
    checks_factory.assert_called_once_with(tmp_path, entry["checks"], simulation=simulation)
    check_runner.run.assert_called_once_with(simulation, checks)


def test_infeasible_entrypoints_preserve_checks_and_skips(monkeypatch, tmp_path):
    import test_unfeasible_problem as module

    functions = {name: function for name, function in vars(module).items()
                 if name.startswith("test_unfeasible_problem_")}
    assert len(functions) == 22
    expected_weeks = {
        "01": [(3, [19]), (6, [42, 43, 49]), (7, [19]), (8, [19])],
        "02": [(6, [22, 23, 29])],
        "03": [(3, [19]), (6, [42, 43, 49]), (7, [19]), (8, [19])],
        "04": [(6, [22, 23, 29])],
        "05": [(10, [19]), (11, [19]), (16, [19]), (29, [19]), (41, [42, 43, 49]),
               (43, [42, 43, 49]), (46, [42, 43, 49]), (142, [42, 43, 49])],
        "06": [(3, [19]), (6, [42, 43, 49]), (7, [19]), (8, [19])],
        "07": [(1, [1]), (3, [52]), (5, [1]), (7, [52]), (9, [52])],
        "08": [(3, [51]), (7, [51]), (9, [51])],
        "09": [(1, [1]), (3, [52]), (5, [1]), (7, [52]), (9, [52])],
        "10": [(3, [51]), (7, [51]), (9, [51])],
    }
    for name, function in functions.items():
        number, behavior = name.removeprefix("test_unfeasible_problem_").split("__")
        simulation = Mock()
        factory = Mock(return_value=simulation)
        check_factory = Mock()
        monkeypatch.setattr(module, "create_simulation", factory)
        monkeypatch.setattr(module, "unfeasible_problem", check_factory)
        check_runner = Mock()
        exe_paths = {}
        function(tmp_path, check_runner, exe_paths)
        factory.assert_called_once_with(
            tmp_path, {"exe": "antares-solver", "options": []}, exe_paths
        )
        assert not (tmp_path / "check-config.json").exists()
        arguments = check_factory.call_args.kwargs
        assert arguments["simulation"] is simulation
        assert arguments["new_behavior"] == behavior.replace("_", "-")
        weeks = expected_weeks[number]
        if behavior.startswith("error"):
            weeks = [(weeks[0][0], weeks[0][1][:1])]
        assert [(week.year, week.weeks) for week in arguments["checks_on_weeks"]] == weeks
        check_runner.run.assert_called_once()
        assert check_runner.run.call_args.args[0] is simulation
        assert check_runner.run.call_args.args[1].check_objects == [check_factory.return_value]
        skipped = any(mark.name == "skip" for mark in function.pytestmark)
        assert skipped == (number in ("09", "10"))


@pytest.mark.parametrize("multi_config,build_type", [
    (False, "Release"), (True, "Release"), (True, ""),
])
def test_cmake_generates_paths_and_two_ctest_entries(tmp_path, multi_config, build_type):
    if not shutil.which("cmake") or not shutil.which("ctest"):
        pytest.skip("CMake and CTest are required")
    if multi_config and not shutil.which("ninja"):
        pytest.skip("Ninja is required for the multi-configuration case")
    source = tmp_path / "source with spaces"
    source.mkdir()
    scripts = source / "study-tests"
    scripts.mkdir()
    shutil.copyfile(SCRIPT_DIR / "CMakeLists.txt", scripts / "CMakeLists.txt")
    (source / "CMakeLists.txt").write_text("""
cmake_minimum_required(VERSION 3.20)
project(study_test_harness NONE)
enable_testing()
foreach(name antares-solver antares-problem-generator)
    add_executable(${name} IMPORTED GLOBAL)
    set_target_properties(${name} PROPERTIES
        IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/bin/${name}"
        IMPORTED_LOCATION_DEBUG "${CMAKE_BINARY_DIR}/Debug/${name}"
        IMPORTED_LOCATION_RELEASE "${CMAKE_BINARY_DIR}/Release/${name}")
endforeach()
function(find_python_module name)
    set(PYTHON_MODULE_${name}_FOUND TRUE PARENT_SCOPE)
endfunction()
add_subdirectory(study-tests)
""")
    build = tmp_path / "build with spaces"
    command = [
        "cmake", "-S", str(source), "-B", str(build),
        f"-DPython3_EXECUTABLE={sys.executable}", f"-DCMAKE_BUILD_TYPE={build_type}",
    ]
    if multi_config:
        command.extend(["-G", "Ninja Multi-Config"])
    subprocess.run(command, check=True, capture_output=True, text=True)
    config = configparser.ConfigParser(interpolation=None)
    config.read(scripts / "executables.ini")
    selected_config = build_type or "Debug"
    for name in ("antares-solver", "antares-problem-generator"):
        assert Path(config["executables"][name]) == build / selected_config / name
    result = subprocess.run(
        ["ctest", "--test-dir", str(build), "-C", selected_config, "--show-only=json-v1"],
        check=True, capture_output=True, text=True,
    )
    tests = json.loads(result.stdout)["tests"]
    assert [test["name"] for test in tests] == ["json", "unfeasible"]
    for test in tests:
        assert test["command"][1:] == [
            "-m", "pytest", f"test_{'from_json' if test['name'] == 'json' else 'unfeasible_problem'}.py",
            "-m", test["name"],
        ]
    labels = next(prop["value"] for prop in tests[1]["properties"] if prop["name"] == "LABELS")
    assert labels == ["sirius", "study"]
