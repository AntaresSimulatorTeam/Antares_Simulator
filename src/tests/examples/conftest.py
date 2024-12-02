import pytest

def pytest_addoption(parser):
    parser.addoption("--solver", action="store", default="sirius")
    parser.addoption("--solver-path", action="store")

@pytest.fixture()
def ortools_solver(request):
    return request.config.getoption("--solver")

@pytest.fixture()
def solver_path(request):
    return request.config.getoption("--solver-path")
