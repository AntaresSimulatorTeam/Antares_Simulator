
find_package(antlr4-runtime CONFIG REQUIRED)
Set(SRCS 
ExprBaseVisitor.h
ExprBaseVisitor.cpp
ExprLexer.h
ExprLexer.cpp
ExprParser.h
ExprParser.cpp
ExprVisitor.h
ExprVisitor.cpp
)

set(lib_name "antlr-interface")
add_library(${lib_name}  ${SRCS})

target_link_libraries(${lib_name} PUBLIC antlr4_shared) # The library or executable that require yaml-cpp library

#ugly see https://github.com/antlr/antlr4/pull/1612
get_target_property(antlr4_lib antlr4_shared IMPORTED_IMPLIB_DEBUG)
set(antlr4_include "${antlr4_lib}/../../../include/antlr4-runtime")
message ("************* ${antlr4_include} ***************** ")

target_include_directories(${lib_name} PUBLIC ${antlr4_include})