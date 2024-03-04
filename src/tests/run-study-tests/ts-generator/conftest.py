import pytest

def pytest_addoption(parser):
    parser.addoption("--exe_tsgenerator_path", action="store", default="")

@pytest.fixture
def exe_tsgenerator_path(request):
    return request.config.getoption("--exe_tsgenerator_path")
