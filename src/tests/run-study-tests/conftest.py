import pytest

def pytest_addoption(parser):
    parser.addoption("--solver", action="store", default="sirius")
    parser.addoption("--antares-simu-path", action="store")
    parser.addoption("--named-mps-problems", action="store_true", default=False)
    parser.addoption("--force-parallel", action="store_true", default=False)
    parser.addoption("--ts-generator", action="store_true", default=False)
    parser.addoption("--api-exe-path", action="store", default="")
    parser.addoption("--write-mps", action="store_true", default=False)
    parser.addoption("--output-dir", action="store", default="20210110-0900eco")

@pytest.fixture()
def solver_name(request):
    return request.config.getoption("--solver")

@pytest.fixture()
def antares_simu_path(request):
    return request.config.getoption("--antares-simu-path")

@pytest.fixture()
def named_mps_problems(request):
    return request.config.getoption("--named-mps-problems")

@pytest.fixture()
def parallel(request):
    return request.config.getoption("--force-parallel")

@pytest.fixture()
def api_exe_path(request):
    return request.config.getoption("--api-exe-path")

@pytest.fixture()
def write_mps(request):
    return request.config.getoption("--write-mps")

@pytest.fixture()
def output_dir(request):
    return request.config.getoption("--output-dir")