project(Expressions)


set(SRC_Expressions
        nodes/PortFieldNode.cpp
        nodes/ComponentNode.cpp

        visitors/PrintVisitor.cpp
        visitors/CloneVisitor.cpp
        visitors/EvalVisitor.cpp

        include/antares/solver/expressions/nodes/ParentNode.h
        include/antares/solver/expressions/nodes/BinaryNode.h
        include/antares/solver/expressions/nodes/AddNode.h
        include/antares/solver/expressions/nodes/MultiplicationNode.h
        include/antares/solver/expressions/nodes/DivisionNode.h
        include/antares/solver/expressions/nodes/SubtractionNode.h
        include/antares/solver/expressions/nodes/ComparisonNode.h
        include/antares/solver/expressions/nodes/EqualNode.h
        include/antares/solver/expressions/nodes/LessThanNode.h
        include/antares/solver/expressions/nodes/LessThanOrEqualNode.h
        include/antares/solver/expressions/nodes/GreaterThanNode.h
        include/antares/solver/expressions/nodes/GreaterThanOrEqualNode.h
        include/antares/solver/expressions/nodes/ComponentNode.h
        include/antares/solver/expressions/nodes/ExpressionsNodes.h
        include/antares/solver/expressions/nodes/LiteralNode.h
        include/antares/solver/expressions/nodes/Leaf.h
        include/antares/solver/expressions/nodes/NegationNode.h
        include/antares/solver/expressions/nodes/Node.h
        include/antares/solver/expressions/nodes/NodesForwardDeclaration.h
        include/antares/solver/expressions/nodes/ParameterNode.h
        include/antares/solver/expressions/nodes/PortFieldNode.h
        include/antares/solver/expressions/nodes/VariableNode.h

        include/antares/solver/expressions/visitors/CloneVisitor.h
        include/antares/solver/expressions/visitors/EvalVisitor.h
        include/antares/solver/expressions/visitors/PrintVisitor.h
        include/antares/solver/expressions/visitors/Visitor.h

        include/antares/solver/expressions/Registry.hxx
        include/antares/solver/expressions/nodes/UnaryNode.h
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

