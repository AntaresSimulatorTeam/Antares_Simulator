import pytest

from retrieve_exe_paths import load_executables

pytest_plugins = ["registered_fixures"]

# This fixture is run only once per session, at scope suggests.
# So the executables are loaded only once, and the same paths are used for all tests.
@pytest.fixture(scope="session")
def exe_paths():
    return load_executables()
