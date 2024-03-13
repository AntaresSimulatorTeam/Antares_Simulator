
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

message ("************* ${ANTLR4_INCLUDE_DIR} ********++++++********* ")

target_include_directories(${lib_name} PUBLIC ${ANTLR4_INCLUDE_DIR})