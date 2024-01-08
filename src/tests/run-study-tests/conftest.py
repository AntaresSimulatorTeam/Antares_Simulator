import pytest

def pytest_addoption(parser):
    parser.addoption("--use-ortools", action="store_true", default=False)
    parser.addoption("--ortools-solver", action="store", default="sirius")
    parser.addoption("--solver-path", action="store")
    parser.addoption("--named-mps-problems", action="store_true", default=False)
    parser.addoption("--force-parallel", action="store_true", default=False)
    parser.addoption("--do-benchmark", action="store_true", default=False)
    parser.addoption("--custom-benchmark-json", action="store_true")

@pytest.fixture()
def ortools_solver(request):
    return request.config.getoption("--ortools-solver")

@pytest.fixture()
def use_ortools(request):
    return request.config.getoption("--use-ortools")

@pytest.fixture()
def solver_path(request):
    return request.config.getoption("--solver-path")

@pytest.fixture()
def named_mps_problems(request):
    return request.config.getoption("--named-mps-problems")

@pytest.fixture()
def parallel(request):
    return request.config.getoption("--force-parallel")

@pytest.fixture()
def do_benchmark(request):
    return request.config.getoption("--do-benchmark")

@pytest.fixture()
def custom_benchmark_json(request):
    return request.config.getoption("--custom-benchmark-json")

