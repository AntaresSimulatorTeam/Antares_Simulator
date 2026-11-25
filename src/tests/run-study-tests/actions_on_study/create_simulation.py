from pathlib import Path

from actions_on_study.study_run import StudyRun, RunStudyWithProblemGenerator, RunStudyWithSolver


def create_simulation(study_path, json_config, exe_paths):
    exe_name = json_config["exe"]
    if exe_name not in ("antares-solver", "antares-problem-generator"):
        raise ValueError(f"Unknown executable {exe_name!r}")

    exe_path = Path(exe_paths[exe_name])

    if not exe_path.is_file():
        raise FileNotFoundError(f"{exe_name} executable not found at {exe_path}; build that target first")

    return make_study_runner(exe_name, exe_path, json_config["options"], study_path)


def make_study_runner(exe_name: str, exe_path: Path, options, study_path) -> StudyRun:
    runner = RunStudyWithSolver if exe_name == "antares-solver" else RunStudyWithProblemGenerator
    return runner(study_path, exe_path, options)
