import pytest

def pytest_addoption(parser):
    parser.addoption("--use-ortools", action="store", default="False")
    parser.addoption("--ortools-solver", action="store", default="sirius")
    parser.addoption("--solver-path", action="store")
    parser.addoption("--study-path", action="store")

@pytest.fixture()
def ortools_solver(request):
    return request.config.getoption("--ortools-solver")

@pytest.fixture()
def use_ortools(request):
    return request.config.getoption("--use-ortools")=="True"
    
@pytest.fixture()
def solver_path(request):
    return request.config.getoption("--solver-path")
    
    
@pytest.fixture()
def study_path(request):
    return request.config.getoption("--study-path")
