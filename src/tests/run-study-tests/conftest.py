import pytest

from executable_paths import load_executables


@pytest.fixture(scope="session")
def exe():
    return load_executables()


def pytest_collection_modifyitems(config, items):
    skipped_studies = ["AreaHydroLevel", "FinalStockEquivalent-cnt", "reservoir-Min-MaxHydroPower"]
    for item in items:
        callspec = getattr(item, "callspec", None)
        test_data = callspec.params.get("test_data", {}) if callspec else {}
        if test_data.get("exe") != "antares-problem-generator":
            continue
        for study_name in skipped_studies:
            if study_name in item.name:
                item.add_marker(
                    pytest.mark.skip(
                        reason=f"Study 'antares-problem-generator can not run {study_name}' because it has not independent weeks"
                    )
                )
                break
