set(SRC
        ortools_utils.h
        ortools_utils.cpp
        filename.h
        filename.cpp
        named_problem.h
        named_problem.cpp
        mps_utils.h
        mps_utils.cpp
        opt_period_string_generator.h
        opt_period_string_generator.cpp
        )

add_library(utils ${SRC})
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
        antares-core
        )
