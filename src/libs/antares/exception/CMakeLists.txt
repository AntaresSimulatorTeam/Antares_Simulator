set(PROJ exception)
set(SRC_PROJ
        AssertionError.cpp
        LoadingError.cpp
        UnfeasibleProblemError.cpp

        antares/exception/AssertionError.hpp
        antares/exception/UnfeasibleProblemError.hpp
        antares/exception/LoadingError.hpp
)
source_group("exception" FILES ${SRC_PROJ})

add_library(${PROJ} ${SRC_PROJ})
add_library(Antares::${PROJ} ALIAS ${PROJ})

target_link_libraries(${PROJ}
        PRIVATE
        yuni-static-core
)

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/libs #fwd
)
