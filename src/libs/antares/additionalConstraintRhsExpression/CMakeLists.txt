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

if (MSVC)
    target_link_libraries(${PROJ} PUBLIC antlr4_shared) # vcpkg triplet x64-windows provides shared lib
else ()
    target_link_libraries(${PROJ} PUBLIC antlr4_static) # vcpkg triplet x64-linux provides static lib
endif ()

target_include_directories(${PROJ}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${ANTLR4_INCLUDE_DIR}>)

install(FILES ${HEADERS}
        DESTINATION "include"
)
