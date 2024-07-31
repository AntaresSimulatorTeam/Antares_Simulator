project(Expressions)


set(SRC_Expressions
        include/antares/solver/expressions/Visitor.h
        include/antares/solver/expressions/Node.h
        include/antares/solver/expressions/Add.h
        include/antares/solver/expressions/Negate.h
        include/antares/solver/expressions/Parameter.h
        include/antares/solver/expressions/PrintVisitor.h
        include/antares/solver/expressions/StringVisitor.h
        include/antares/solver/expressions/CloneVisitor.h

)


source_group("expressions" FILES ${SRC_Expressions})
add_library(antares-solver-expressions
        ${SRC_Expressions})

target_include_directories(antares-solver-expressions
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(DIRECTORY include/antares
        DESTINATION "include"
)

add_executable(visitor main.cpp

)
