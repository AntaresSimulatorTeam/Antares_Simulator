from pathlib import Path

from actions_on_study.antares_problem_generator import AntaresProblemGeneratorRun
from actions_on_study.study_run import study_run


def create_simulation(study_path, json_config, exe_paths):
    name = json_config["exe"]
    if name not in ("antares-solver", "antares-problem-generator"):
        raise ValueError(f"Unknown executable {name!r}")

    path = Path(exe_paths[name])

    if not path.is_file():
        raise FileNotFoundError(f"{name} executable not found at {path}; build that target first")

    runner = study_run if name == "antares-solver" else AntaresProblemGeneratorRun
    return runner(study_path, path, json_config["options"])
