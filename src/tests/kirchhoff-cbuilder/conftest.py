import pytest

def pytest_addoption(parser):
    parser.addoption("--exe_kirchhoff_path", action="store", default="")

@pytest.fixture
def exe_kirchhoff_path(request):
    return request.config.getoption("--exe_kirchhoff_path")
