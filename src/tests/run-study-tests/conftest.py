import pytest


def pytest_addoption(parser):
    parser.addoption("--solver", action="store", default="sirius")
    parser.addoption("--antares-simu-path", action="store")
    parser.addoption("--named-mps-problems", action="store_true", default=False)
    parser.addoption("--force-parallel", action="store_true", default=False)
    parser.addoption("--ts-generator", action="store_true", default=False)
    parser.addoption("--antares-problem-generator-exe", action="store", default="")


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
def antares_problem_generator_exe(request):
    return request.config.getoption("--antares-problem-generator-exe")


def pytest_collection_modifyitems(config, items):
    """
    Hook to skip specific failing studies.
    """
    skipped_studies = ["AreaHydroLevel", "FinalStockEquivalent-cnt", "reservoir-Min-MaxHydroPower", ]
    if config.getoption("--antares-problem-generator-exe"):
        for item in items:
            for study_name in skipped_studies:
                if study_name in item.name:
                    item.add_marker(
                        pytest.mark.skip(
                            reason=f"Study 'antares-problem-generator can not run {study_name}' because it has not independent weeks"
                        )
                    )
                    break
