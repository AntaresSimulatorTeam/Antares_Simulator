find_package(ortools QUIET)
if (NOT ortools_FOUND OR BUILD_ORTOOLS)
    message(STATUS "OR-Tools tag ${ORTOOLS_TAG}")
    FetchContent_Declare(ortools
            GIT_REPOSITORY "https://github.com/rte-france/or-tools-rte"
            GIT_TAG ${ORTOOLS_TAG}
            GIT_SHALLOW TRUE
    )

    # Pass options to OR-Tools's CMake
    set(BUILD_DEPS "ON" CACHE INTERNAL "")
    set(BUILD_SAMPLES "OFF" CACHE INTERNAL "")
    set(BUILD_FLATZINC "OFF" CACHE INTERNAL "")
    set(BUILD_EXAMPLES "OFF" CACHE INTERNAL "")
    set(USE_SCIP "ON" CACHE INTERNAL "")
    set(USE_GLPK "ON" CACHE INTERNAL "")
    # We build OR-Tools as a static lib. Cyclic dependencies are detected
    # without this flag.
    set(BUILD_SHARED_LIBS "OFF" CACHE INTERNAL "")
    # In mode optimization error analysis, we call Sirius through or-tools
    # So we need to activate Sirius in or-tools configuration (OFF by default)
    set(USE_SIRIUS "ON" CACHE INTERNAL "")

    FetchContent_MakeAvailable(ortools)
endif ()