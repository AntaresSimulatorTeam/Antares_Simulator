import pytest

def pytest_addoption(parser):
    parser.addoption("--use-ortools", action="store", default=False)
    parser.addoption("--ortools-solver", action="store", default="sirius")

@pytest.fixture()
def ortools_solver(request):
    return request.config.getoption("--ortools-solver")

@pytest.fixture()
def use_ortools(request):
    return request.config.getoption("--use-ortools")
