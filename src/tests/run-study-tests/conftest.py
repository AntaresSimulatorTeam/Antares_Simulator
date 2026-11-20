import pytest

from executable_paths import load_executables

pytest_plugins = ["registered_fixures"]


@pytest.fixture(scope="session")
def exe_paths():
    return load_executables()
