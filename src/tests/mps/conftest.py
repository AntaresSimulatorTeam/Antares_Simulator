import pytest

def pytest_addoption(parser):
    parser.addoption("--use-ortools", action="store_true", default=False)
    parser.addoption("--ortools-solver", action="store", default="sirius")
    parser.addoption("--solver-path", action="store")

@pytest.fixture()
def ortools_solver(request):
    return request.config.getoption("--ortools-solver")

@pytest.fixture()
def use_ortools(request):
    return request.config.getoption("--use-ortools")

@pytest.fixture()
def solver_path(request):
    return request.config.getoption("--solver-path")
