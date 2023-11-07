set(SRC
        include/antares/solver/utils/ortools_utils.h
        ortools_utils.cpp
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
        )

add_library(utils ${SRC})
add_library(Antares::solverUtils ALIAS utils)
if(NOT MSVC)
    target_compile_options(utils
            PUBLIC
            "-Wno-unused-variable")
else()
    #set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS} /wd 4101") # unused local variable
    #set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} /wd 4101") # unused local variable
endif()
target_link_libraries(utils
        PUBLIC
        ortools::ortools
        sirius_solver
        Antares::study
        Antares::result_writer #ortools_utils.h
        antares-core #enum.h
        )

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
