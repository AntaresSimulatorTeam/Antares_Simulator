project(Expressions)


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
	include/antares/solver/expressions/NodeRegistry.h
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
        include/antares/solver/expressions/nodes/PortFieldSumNode.h
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
        include/antares/solver/expressions/visitors/PortFieldSubstitutionVisitor.h
        include/antares/solver/expressions/visitors/PortFieldSumSubstitutionVisitor.h
        include/antares/solver/expressions/visitors/AstDOTStyleVisitor.h
        include/antares/solver/expressions/visitors/InvalidNode.h

        include/antares/solver/expressions/Registry.hxx
        include/antares/solver/expressions/IName.h
        include/antares/solver/expressions/hashable.h
)

source_group("expressions" FILES ${SRC_Expressions})
add_library(solver-expressions
        ${SRC_Expressions})
add_library(Antares::solver-expressions ALIAS solver-expressions)

target_include_directories(solver-expressions
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(solver-expressions
        PUBLIC
        Boost::headers
        Antares::logs
)


add_library(solver-expressions-iterators
        iterators/pre-order.cpp
        include/antares/solver/expressions/iterators/pre-order.h
)

target_link_libraries(solver-expressions-iterators PRIVATE solver-expressions)

install(DIRECTORY include/antares
        DESTINATION "include"
)

