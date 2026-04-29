# Antl4 parser
set(PROJ scenario-builder-antlr)
set(HEADERS
        ScenarioBuilderBaseVisitor.h
        ScenarioBuilderLexer.h
        ScenarioBuilderParser.h
        ScenarioBuilderVisitor.h
)

Set(SRCS
        ScenarioBuilderBaseVisitor.cpp
        ScenarioBuilderLexer.cpp
        ScenarioBuilderParser.cpp
        ScenarioBuilderVisitor.cpp
)

add_library(${PROJ} ${SRCS})
add_library(Antares::${PROJ} ALIAS ${PROJ})

link_antlr4(${PROJ})

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${ANTLR4_INCLUDE_DIR}>)

install(FILES ${HEADERS}
        DESTINATION "include"
)