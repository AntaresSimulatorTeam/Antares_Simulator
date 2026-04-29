add_subdirectory(scenarioBuilderExpression)

add_library(scenario-group-parser)
add_library(Antares::scenario-group-parser ALIAS scenario-group-parser)
target_sources(scenario-group-parser
        PRIVATE
        include/antares/scenarioGroupParser/ScenarioGroupParser.h
        ScenarioGroupParser.cpp
)

target_include_directories(scenario-group-parser
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(scenario-group-parser
        PUBLIC
        Antares::scenario-builder-antlr
        PRIVATE
        fmt::fmt
        Antares::exception
)

install(DIRECTORY include/antares
        DESTINATION "include"
)