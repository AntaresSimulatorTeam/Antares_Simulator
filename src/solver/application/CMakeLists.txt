set(HEADERS
        include/antares/application/application.h
        include/antares/application/ScenarioBuilderOwner.h
)
set(SRC_APPLICATION
        ${HEADERS}
        application.cpp
        process-priority.cpp
        ScenarioBuilderOwner.cpp
)
source_group("application" FILES ${SRC_APPLICATION})

add_library(application
        ${SRC_APPLICATION}
)
add_library(Antares::application ALIAS application)

target_link_libraries(application
        PUBLIC
        yuni-static-core
        Antares::study
        Antares::infoCollection
        solver-lib
        PRIVATE
        Antares::logs
        Antares::sys
        Antares::solverUtils
        Antares::signal-handling
        Antares::resources
        Antares::file-tree-study-loader
)

target_include_directories(application
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)