import sys
from pathlib import Path

# Add the parent directory (tests) to sys.path so we can import shared_utils
tests_dir = Path(__file__).parents[2]
if str(tests_dir) not in sys.path:
    sys.path.insert(0, str(tests_dir))
