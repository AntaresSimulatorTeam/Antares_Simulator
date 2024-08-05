project(Expressions)


set(SRC_Expressions
        include/antares/solver/expressions/Visitor.h
        include/antares/solver/expressions/Node.h
        AddNode.cpp
        include/antares/solver/expressions/AddNode.h
        Negate.cpp
        include/antares/solver/expressions/Negate.h
        PrintVisitor.cpp
        include/antares/solver/expressions/PrintVisitor.h
        CloneVisitor.cpp
        include/antares/solver/expressions/CloneVisitor.h
        include/antares/solver/expressions/ExpressionsNodes.h
        Leaf.cpp
        include/antares/solver/expressions/Leaf.h
        include/antares/solver/expressions/NodesForwardDeclaration.h
        EvalVisitor.cpp
        include/antares/solver/expressions/EvalVisitor.h
        include/antares/solver/expressions/PortFieldNode.h
        PortFieldNode.cpp
)


source_group("expressions" FILES ${SRC_Expressions})
add_library(antares-solver-expressions
        ${SRC_Expressions})

target_include_directories(antares-solver-expressions
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(antares-solver-expressions
        PUBLIC
        Antares::logs
)
install(DIRECTORY include/antares
        DESTINATION "include"
)

add_executable(visitor main.cpp)
target_link_libraries(visitor PRIVATE antares-solver-expressions)
