set(PROJ exception)
set(SRC_PROJ
        AssertionError.cpp
        LoadingError.cpp
        UnfeasibleProblemError.cpp

        include/antares/exception/AssertionError.hpp
        include/antares/exception/InitializationError.hpp
        include/antares/exception/InvalidArgumentError.hpp
        include/antares/exception/LoadingError.hpp
        include/antares/exception/UnfeasibleProblemError.hpp
)
source_group("exception" FILES ${SRC_PROJ})

add_library(${PROJ} ${SRC_PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})

# libexception.a is linked into the SHARED library simulation-table-writers, so
# its objects must be position-independent. (Restored after "Revert feat: pch"
# dropped this together with the PCH work.)
set_target_properties(${PROJ} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
