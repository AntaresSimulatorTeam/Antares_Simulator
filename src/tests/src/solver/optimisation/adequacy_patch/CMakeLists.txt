include(${CMAKE_SOURCE_DIR}/src/tests/macros.cmake)

add_boost_test(tests-adq-patch
        SRC adequacy_patch.cpp
        test_compute_gems_contribution.cpp
        LIBS
        model_antares
        array
        antares-study-system-model
        Antares::linear-problem-mpsolver-impl
        Antares::optim-model-filler
        Antares::linear-problem-data-impl
        Antares::yml-model
        Antares::model-converter
        Antares::yml-system
        Antares::simulation-table
        test_utils_unit)
