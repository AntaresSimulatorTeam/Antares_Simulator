set(SRC
        include/antares/solver/utils/ortools_utils.h
        ortools_utils.cpp
        include/antares/solver/utils/ortools_quadratic_wrapper.h
        ortools_quadratic_wrapper.cpp
        include/antares/solver/utils/filename.h
        filename.cpp
        include/antares/solver/utils/named_problem.h
        named_problem.cpp
        include/antares/solver/utils/mps_utils.h
        mps_utils.cpp
        include/antares/solver/utils/name_translator.h
        name_translator.cpp
        include/antares/solver/utils/opt_period_string_generator.h
        opt_period_string_generator.cpp
        basis_status.cpp
        include/antares/solver/utils/basis_status.h
        basis_status_impl.cpp
        basis_status_impl.h
)

add_library(utils ${SRC})
add_library(Antares::solverUtils ALIAS utils)
if (NOT MSVC)
    target_compile_options(utils
            PUBLIC
            "-Wno-unused-variable")
endif ()
target_link_libraries(utils
        PUBLIC
        # Until issue here is resolved, link the whole archive: https://github.com/google/or-tools/discussions/4538
        # TODO: keep an eye out for bug resolution in future versions of ortools (> 9.12)
        $<LINK_LIBRARY:WHOLE_ARCHIVE,ortools::ortools>
        sirius_solver
        Antares::study
        Antares::result_writer
        Antares::optimization-options
        antares-core
)

target_include_directories(utils
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
