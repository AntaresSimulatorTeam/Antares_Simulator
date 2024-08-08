include(FetchContent)
find_package(sirius_solver QUIET)
if (NOT sirius_solver_FOUND)
    include(FetchContent)
    message("SIRIUS not found, fetching it from github")
    FetchContent_Declare(sirius_solver
            GIT_REPOSITORY https://github.com/AntaresSimulatorTeam/sirius-solver.git
            GIT_TAG feature/fix_include_path
            SOURCE_SUBDIR src
            OVERRIDE_FIND_PACKAGE ON
    )
    FetchContent_MakeAvailable(sirius_solver)
endif ()
find_package(sirius_solver REQUIRED)
