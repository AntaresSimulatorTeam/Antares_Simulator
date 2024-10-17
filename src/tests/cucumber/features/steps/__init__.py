from import_helper import import_steps
from pathlib import Path

STEPS_DIR_NAME = "steps"
STEPS_DIR = Path(__file__).parent
assert STEPS_DIR.name == STEPS_DIR_NAME

import_steps(STEPS_DIR)
