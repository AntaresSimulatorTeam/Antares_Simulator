set(PROJ antlr-interface)
set(HEADERS
        include/antares/antlr-interface/ExprBaseVisitor.h
        include/antares/antlr-interface/ExprLexer.h
        include/antares/antlr-interface/ExprParser.h
        include/antares/antlr-interface/ExprVisitor.h
)

Set(SRCS
        ExprBaseVisitor.cpp
        ExprLexer.cpp
        ExprParser.cpp
        ExprVisitor.cpp
)
find_package(antlr4-runtime CONFIG REQUIRED)

add_library(${PROJ} ${SRCS})
add_library(Antares::${PROJ} ALIAS ${PROJ})

if (MSVC)
    target_link_libraries(${PROJ} PUBLIC antlr4_shared) # vcpkg triplet x64-windows provides shared lib
else ()
    target_link_libraries(${PROJ} PUBLIC antlr4_static) # vcpkg triplet x64-linux provides static lib
endif ()

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${ANTLR4_INCLUDE_DIR})

install(DIRECTORY include/antares
        DESTINATION "include"
)
