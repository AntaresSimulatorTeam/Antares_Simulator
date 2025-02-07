project(Expressions)

add_subdirectory(antlr-interface)
set(SRC_Expressions
        nodes/PortFieldNode.cpp
        nodes/PortFieldSumNode.cpp
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
        visitors/PortFieldSubstitutionVisitor.cpp
        visitors/PortFieldSumSubstitutionVisitor.cpp
        visitors/AstDOTStyleVisitor.cpp
        visitors/InvalidNode.cpp
        visitors/NodeVisitor.cpp

        hashable.cpp

        NodeRegistry.cpp
        include/antares/expressions/NodeRegistry.h
        include/antares/expressions/nodes/SumNode.h
        include/antares/expressions/nodes/BinaryNode.h
        include/antares/expressions/nodes/ComparisonNode.h
        include/antares/expressions/nodes/ComponentNode.h
        include/antares/expressions/nodes/DivisionNode.h
        include/antares/expressions/nodes/EqualNode.h
        include/antares/expressions/nodes/ExpressionsNodes.h
        include/antares/expressions/nodes/GreaterThanOrEqualNode.h
        include/antares/expressions/nodes/Leaf.h
        include/antares/expressions/nodes/LessThanOrEqualNode.h
        include/antares/expressions/nodes/LiteralNode.h
        include/antares/expressions/nodes/MultiplicationNode.h
        include/antares/expressions/nodes/NegationNode.h
        include/antares/expressions/nodes/Node.h
        include/antares/expressions/nodes/NodesForwardDeclaration.h
        include/antares/expressions/nodes/ParameterNode.h
        include/antares/expressions/nodes/PortFieldNode.h
        include/antares/expressions/nodes/PortFieldSumNode.h
        include/antares/expressions/nodes/SubtractionNode.h
        include/antares/expressions/nodes/UnaryNode.h
        include/antares/expressions/nodes/VariableNode.h

        include/antares/expressions/visitors/CloneVisitor.h
        include/antares/expressions/visitors/CompareVisitor.h
        include/antares/expressions/visitors/EvalVisitor.h
        include/antares/expressions/visitors/EvaluationContext.h
        include/antares/expressions/visitors/LinearStatus.h
        include/antares/expressions/visitors/LinearityVisitor.h
        include/antares/expressions/visitors/NodeVisitor.h
        include/antares/expressions/visitors/PrintVisitor.h
        include/antares/expressions/visitors/TimeIndexVisitor.h
        include/antares/expressions/visitors/TimeIndex.h
        include/antares/expressions/visitors/SubstitutionVisitor.h
        include/antares/expressions/visitors/PortFieldSubstitutionVisitor.h
        include/antares/expressions/visitors/PortFieldSumSubstitutionVisitor.h
        include/antares/expressions/visitors/AstDOTStyleVisitor.h
        include/antares/expressions/visitors/InvalidNode.h

        include/antares/expressions/Registry.hxx
        include/antares/expressions/IName.h
        include/antares/expressions/hashable.h
        include/antares/expressions/expression.h
)

source_group("expressions" FILES ${SRC_Expressions})
add_library(expressions
        ${SRC_Expressions})
add_library(Antares::expressions ALIAS expressions)

target_include_directories(expressions
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(expressions
        PUBLIC
        Boost::headers
        Antares::logs
)


add_library(expressions-iterators
        iterators/pre-order.cpp
        include/antares/expressions/iterators/pre-order.h
)

target_link_libraries(expressions-iterators PRIVATE expressions)

install(DIRECTORY include/antares
        DESTINATION "include"
)

