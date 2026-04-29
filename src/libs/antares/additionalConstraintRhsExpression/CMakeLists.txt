set(PROJ additional-constraint-rhs-antlr-interface)
set(HEADERS
        HoursFieldBaseVisitor.h
        HoursFieldLexer.h
        HoursFieldParser.h
        HoursFieldVisitor.h
)

Set(SRCS
        HoursFieldBaseVisitor.cpp
        HoursFieldLexer.cpp
        HoursFieldParser.cpp
        HoursFieldVisitor.cpp
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
