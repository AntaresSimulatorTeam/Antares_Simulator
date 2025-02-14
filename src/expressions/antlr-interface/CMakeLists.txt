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

if (MSVC)
    target_link_libraries(${PROJ} PUBLIC antlr4_shared) # vcpkg triplet x64-windows provides shared lib
else ()
    target_link_libraries(${PROJ} PUBLIC antlr4_static) # vcpkg triplet x64-linux provides static lib
endif ()

target_include_directories(${PROJ}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ANTLR4_INCLUDE_DIR})

install(FILES ${HEADERS}
        DESTINATION "include"
)
