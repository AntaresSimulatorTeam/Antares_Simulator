set(PROJ antlr-interface)
set(HEADERS
        ExprBaseVisitor.h
        ExprLexer.h
        ExprParser.h
        ExprVisitor.h
)

Set(SRCS
        ExprBaseVisitor.cpp
        ExprLexer.cpp
        ExprParser.cpp
        ExprVisitor.cpp
)

add_library(${PROJ} ${SRCS})
add_library(Antares::${PROJ} ALIAS ${PROJ})
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJ} PRIVATE
            "-Wno-unused-parameter")
endif ()

link_antlr4(${PROJ})

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${ANTLR4_INCLUDE_DIR}>)

install(FILES ${HEADERS}
        DESTINATION "include"
)
