import pytest

def pytest_addoption(parser):
    parser.addoption("--tsgenerator_path", action="store", default="")

@pytest.fixture
def tsgenerator_path(request):
    return request.config.getoption("--tsgenerator_path")
