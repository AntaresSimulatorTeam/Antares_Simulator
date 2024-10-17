from import_helper import import_steps_from_subdirs, import_steps
from pathlib import Path

STEPS_DIR_NAME = "steps"
STEPS_DIR = Path(__file__).parent
assert STEPS_DIR.name == STEPS_DIR_NAME

import_steps(STEPS_DIR)

# import_steps_from_subdirs("Antares_Simulator/src/tests/cucumber/features/steps")

# import_module(".assertions", "Antares_Simulator.src.tests.cucumber.features.steps")
# import_module(
#     ".study_output_handler", "Antares_Simulator.src.tests.cucumber.features.steps"
# )
# import_module(
#     ".study_input_handler", "Antares_Simulator.src.tests.cucumber.features.steps"
# )
# import_module(".simulator_utils", "Antares_Simulator.src.tests.cucumber.features.steps")
# import_module(".steps", "Antares_Simulator.src.tests.cucumber.features.steps")
