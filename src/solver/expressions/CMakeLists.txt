project(Expressions)


set(SRC_Expressions
        nodes/PortFieldNode.cpp
        nodes/ComponentNode.cpp
        nodes/BinaryNode.cpp
        nodes/UnaryNode.cpp
        nodes/SumNode.cpp

        visitors/CloneVisitor.cpp
        visitors/CompareVisitor.cpp
        visitors/EvalVisitor.cpp
        visitors/EvaluationContext.cpp
        visitors/LinearityVisitor.cpp
        visitors/TimeIndexVisitor.cpp
        visitors/PrintVisitor.cpp
        visitors/SubstitutionVisitor.cpp
        visitors/InvalidNode.cpp

        include/antares/solver/expressions/nodes/SumNode.h
        include/antares/solver/expressions/nodes/BinaryNode.h
        include/antares/solver/expressions/nodes/ComparisonNode.h
        include/antares/solver/expressions/nodes/ComponentNode.h
        include/antares/solver/expressions/nodes/DivisionNode.h
        include/antares/solver/expressions/nodes/EqualNode.h
        include/antares/solver/expressions/nodes/ExpressionsNodes.h
        include/antares/solver/expressions/nodes/GreaterThanOrEqualNode.h
        include/antares/solver/expressions/nodes/Leaf.h
        include/antares/solver/expressions/nodes/LessThanOrEqualNode.h
        include/antares/solver/expressions/nodes/LiteralNode.h
        include/antares/solver/expressions/nodes/MultiplicationNode.h
        include/antares/solver/expressions/nodes/NegationNode.h
        include/antares/solver/expressions/nodes/Node.h
        include/antares/solver/expressions/nodes/NodesForwardDeclaration.h
        include/antares/solver/expressions/nodes/ParameterNode.h
        include/antares/solver/expressions/nodes/PortFieldNode.h
        include/antares/solver/expressions/nodes/SubtractionNode.h
        include/antares/solver/expressions/nodes/UnaryNode.h
        include/antares/solver/expressions/nodes/VariableNode.h

        include/antares/solver/expressions/visitors/CloneVisitor.h
        include/antares/solver/expressions/visitors/CompareVisitor.h
        include/antares/solver/expressions/visitors/EvalVisitor.h
        include/antares/solver/expressions/visitors/EvaluationContext.h
        include/antares/solver/expressions/visitors/LinearStatus.h
        include/antares/solver/expressions/visitors/LinearityVisitor.h
        include/antares/solver/expressions/visitors/NodeVisitor.h
        include/antares/solver/expressions/visitors/PrintVisitor.h
        include/antares/solver/expressions/visitors/TimeIndexVisitor.h
        include/antares/solver/expressions/visitors/TimeIndex.h
        include/antares/solver/expressions/visitors/SubstitutionVisitor.h
        include/antares/solver/expressions/visitors/InvalidNode.h

        include/antares/solver/expressions/Registry.hxx
        include/antares/solver/expressions/IName.h
)

source_group("expressions" FILES ${SRC_Expressions})
add_library(antares-solver-expressions
        ${SRC_Expressions})
add_library(Antares::antares-solver-expressions ALIAS antares-solver-expressions)

target_include_directories(antares-solver-expressions
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(antares-solver-expressions
        PUBLIC
        Antares::logs
)


add_library(antares-solver-expressions-iterators
        iterators/pre-order.cpp
        include/antares/solver/expressions/iterators/pre-order.h
)

target_link_libraries(antares-solver-expressions-iterators PRIVATE antares-solver-expressions)

install(DIRECTORY include/antares
        DESTINATION "include"
)

