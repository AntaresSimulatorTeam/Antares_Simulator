from pathlib import Path

import pytest

from actions_on_study.create_simulation import create_simulation
from parse_studies.look_for_studies import look_for_studies
from parse_studies.json_collector import jsonCollector
from check_on_results.create_checks import create_checks
from parse_studies.cucumber_collector import cucumberCollector

ROOT_FOLDER = Path(__file__).resolve().parents[3] / 'resources' / 'batches'

# Collect names of tests that were migrated to cucumber
cucumber_collector = cucumberCollector(Path(__file__).resolve().parent.parent / 'cucumber')
cucumber_studies = cucumber_collector.collect()

# Search for studies inside directory ROOT_FOLDER and collects all their paths.
# Each study is supposed to hold a check-config.json file, containing
# all data to make particular checks on the associated study.
# Skip studies already migrated to cucumber.
study_paths = look_for_studies(ROOT_FOLDER, cucumber_studies)

# Collects json files in each study and retrieves the checks data from each of them.
json_collector = jsonCollector(study_paths)
json_collector.collect()

@pytest.mark.json
@pytest.mark.parametrize('study_path, json_config', json_collector.pairs(), ids=json_collector.testIds())
def test(study_path, json_config, check_runner, exe):
    simulation = create_simulation(study_path, json_config, exe)
    checks = create_checks(study_path, json_config["checks"], simulation=simulation)
    check_runner.run(simulation, checks)
