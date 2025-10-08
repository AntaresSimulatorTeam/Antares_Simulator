project(Expressions)

add_subdirectory(antlr-interface)
set(SRC_Expressions
        nodes/PortFieldNode.cpp
        nodes/PortFieldSumNode.cpp
        nodes/BinaryNode.cpp
        nodes/UnaryNode.cpp
        nodes/ParentNode.cpp
        nodes/TimeSumNode.cpp


        visitors/CloneVisitor.cpp
        visitors/CompareVisitor.cpp
        visitors/EvalVisitor.cpp
        visitors/LinearityVisitor.cpp
        visitors/TimeIndexVisitor.cpp
        visitors/PrintVisitor.cpp
        visitors/AstDOTStyleVisitor.cpp
        visitors/InvalidNode.cpp
        visitors/NodeVisitor.cpp


        NodeRegistry.cpp
        include/antares/expressions/NodeRegistry.h
        include/antares/expressions/nodes/SumNode.h
        include/antares/expressions/nodes/ParentNode.h
        include/antares/expressions/nodes/BinaryNode.h
        include/antares/expressions/nodes/ComparisonNode.h
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
        include/antares/expressions/nodes/TimeIndexNode.h


        include/antares/expressions/visitors/CloneVisitor.h
        include/antares/expressions/visitors/CompareVisitor.h
        include/antares/expressions/visitors/EvalVisitor.h
        include/antares/expressions/visitors/LinearStatus.h
        include/antares/expressions/visitors/LinearityVisitor.h
        include/antares/expressions/visitors/NodeVisitor.h
        include/antares/expressions/visitors/PrintVisitor.h
        include/antares/expressions/visitors/TimeIndexVisitor.h
        include/antares/expressions/visitors/AstDOTStyleVisitor.h
        include/antares/expressions/visitors/InvalidNode.h

        include/antares/expressions/Registry.hxx
        include/antares/expressions/IName.h
        include/antares/expressions/expression.h
        include/antares/expressions/ShiftVector.h
        include/antares/expressions/nodes/TimeShiftNode.h
        include/antares/expressions/nodes/TimeSumNode.h
        nodes/TimeSumNode.cpp
        include/antares/expressions/RotateIndex.h
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
        #TODO i need LinearProblemData
        linear-problem-api
        Antares::modeler-optimisation-container
        PRIVATE
        Antares::antares-study-system-model
        Antares::exception
)


add_library(expressions-iterators
        iterators/pre-order.cpp
        include/antares/expressions/iterators/pre-order.h
)
add_library(Antares::expressions-iterators ALIAS expressions-iterators)

target_link_libraries(expressions-iterators PRIVATE expressions)

install(DIRECTORY include/antares
        DESTINATION "include"
)

