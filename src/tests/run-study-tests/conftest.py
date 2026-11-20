import pytest

from retrieve_exe_paths import load_executables

pytest_plugins = ["registered_fixures"]


@pytest.fixture(scope="session")
def exe_paths():
    return load_executables()
