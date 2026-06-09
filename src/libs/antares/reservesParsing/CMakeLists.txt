set(PROJ reserves-parsing-antlr-interface)
set(HEADERS
        SymmetryFieldBaseVisitor.h
        SymmetryFieldLexer.h
        SymmetryFieldParser.h
        SymmetryFieldVisitor.h
)

Set(SRCS
        SymmetryFieldBaseVisitor.cpp
        SymmetryFieldLexer.cpp
        SymmetryFieldParser.cpp
        SymmetryFieldVisitor.cpp
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
