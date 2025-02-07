set(PROJ exception)
set(SRC_PROJ
        AssertionError.cpp
        LoadingError.cpp
        UnfeasibleProblemError.cpp

        include/antares/exception/AssertionError.hpp
        include/antares/exception/InitializationError.hpp
        include/antares/exception/LoadingError.hpp
        include/antares/exception/UnfeasibleProblemError.hpp
)
source_group("exception" FILES ${SRC_PROJ})

add_library(${PROJ} ${SRC_PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PUBLIC
        Antares::study
        PRIVATE
        yuni-static-core
)

target_include_directories(${PROJ}
                PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)
