// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <algorithm>
#include <ranges>
#include <typeindex>
#include <unordered_map>

#include <antares/expressions/IName.h>
#include <antares/expressions/nodes/FunctionNode.h>
#include <antares/expressions/nodes/Node.h>
#include <antares/expressions/nodes/NodesForwardDeclaration.h>
#include <antares/expressions/visitors/InvalidNode.h>

namespace Antares::Expressions::Visitors
{
// we use logError because the inclusion of <antares/logs/logs.h> somehow results in the
// inclusion of <windows.h> (very bad idea in a header!) which conflict with antlr4 headers (defines
// in the former become enums in the latter etc...)
void logError(const std::string& msg);

template<class RetT, class VisitorT, class NodeT, class... Args>
RetT tryVisit(const Nodes::Node* node, VisitorT& visitor, Args... args)
{
    auto* x = dynamic_cast<const NodeT*>(node);
    return visitor.visit(x, args...);
}

template<class R, class... Args>
class NodeVisitor;

template<class R, class... Args>
struct NodeVisitsProvider
{
    using FunctionT = R (*)(const Nodes::Node*, NodeVisitor<R, Args...>&, Args... args);

    /**
     * Creates a map associating node types with corresponding visitor functions.
     *
     * @tparam NodeTypes A variadic pack of node types to be included in the map.
     * @return An `std::unordered_map` containing the associations between node types and their
     * corresponding visitor functions.
     */
    template<class... NodeTypes>
    static auto NodesVisitList()
    {
        std::unordered_map<std::type_index, FunctionT> nodeDispatchFunctions;
        (
          [&nodeDispatchFunctions] {
              nodeDispatchFunctions[typeid(NodeTypes)] = &tryVisit<R,
                                                                   NodeVisitor<R, Args...>,
                                                                   NodeTypes>;
          }(),
          ...);
        return nodeDispatchFunctions;
    }
};

template<class R, class... Args>
class NodeVisitor: public IName
{
public:
    ~NodeVisitor() override = default;

    /**
     * Dispatches a node to an appropriate visitor function based on its type.
     *
     * This method uses a map that associates node types
     * with their corresponding visitor functions. It attempts to find the visitor function
     * for the provided `node`. If a match is found, the corresponding
     * visitor function is called with the node, and any
     * additional arguments (`args...`).
     *
     * @param node A pointer to the Node object to be visited.
     * @param args Variadic template arguments to be passed to the visitor functions.
     * @return The return value of the visitor function.
     *
     */
    R dispatch(const Nodes::Node* node, Args... args)
    {
        if (!node)
        {
            throw InvalidNode("null");
        }

        const static auto nodeVisitList = NodeVisitsProvider<R, Args...>::template NodesVisitList<
          Nodes::SumNode,
          Nodes::SubtractionNode,
          Nodes::MultiplicationNode,
          Nodes::DivisionNode,
          Nodes::EqualNode,
          Nodes::LessThanOrEqualNode,
          Nodes::GreaterThanOrEqualNode,
          Nodes::NegationNode,
          Nodes::ParameterNode,
          Nodes::VariableNode,
          Nodes::LiteralNode,
          Nodes::PortFieldNode,
          Nodes::PortFieldSumNode,
          Nodes::TimeShiftNode,
          Nodes::TimeIndexNode,
          Nodes::TimeSumNode,
          Nodes::TPlusNode,
          Nodes::AllTimeSumNode,
          Nodes::FunctionNode>();

        try
        {
            return nodeVisitList.at(typeid(*node))(node, *this, args...);
        }
        catch (std::exception& e)
        {
            using namespace std::string_literals;
            logError("Antares::Expressions::Visitor: could not visit the node! "s + e.what());
            throw;
        }
    }

    /**
     * @brief Visits a SumNode and processes its children.
     *
     * @param node A pointer to the SumNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the SumNode.
     */
    virtual R visit(const Nodes::SumNode*, Args... args) = 0;
    /**
     * @brief Visits a SubtractionNode and processes its children.
     *
     * @param node A pointer to the SubtractionNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the SubtractionNode.
     */
    virtual R visit(const Nodes::SubtractionNode*, Args... args) = 0;
    /**
     * @brief Visits a MultiplicationNode and processes its children.
     *
     * @param node A pointer to the MultiplicationNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the MultiplicationNode.
     */
    virtual R visit(const Nodes::MultiplicationNode*, Args... args) = 0;
    /**
     * @brief Visits a DivisionNode and processes its children.
     *
     * @param node A pointer to the DivisionNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the DivisionNode.
     */
    virtual R visit(const Nodes::DivisionNode*, Args... args) = 0;
    /**
     * @brief Visits an EqualNode and processes its children.
     *
     * @param node A pointer to the EqualNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the EqualNode.
     */
    virtual R visit(const Nodes::EqualNode*, Args... args) = 0;

    /**
     * @brief Visits a LessThanOrEqualNode and processes its children.
     *
     * @param node A pointer to the LessThanOrEqualNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the LessThanOrEqualNode.
     */
    virtual R visit(const Nodes::LessThanOrEqualNode*, Args... args) = 0;

    /**
     * @brief Visits a GreaterThanOrEqualNode and processes its children.
     *
     * @param node A pointer to the GreaterThanOrEqualNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the GreaterThanOrEqualNode.
     */
    virtual R visit(const Nodes::GreaterThanOrEqualNode*, Args... args) = 0;

    /**
     * @brief Visits a NegationNode and processes its child.
     *
     * @param node A pointer to the NegationNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the NegationNode.
     */
    virtual R visit(const Nodes::NegationNode*, Args... args) = 0;

    /**
     * @brief Visits a LiteralNode.
     *
     * @param node A pointer to the LiteralNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the LiteralNode.
     */
    virtual R visit(const Nodes::LiteralNode*, Args... args) = 0;

    /**
     * @brief Visits a VariableNode.
     *
     * @param node A pointer to the VariableNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the VariableNode.
     */
    virtual R visit(const Nodes::VariableNode*, Args... args) = 0;

    /**
     * @brief Visits a ParameterNode.
     *
     * @param node A pointer to the ParameterNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the ParameterNode.
     */
    virtual R visit(const Nodes::ParameterNode*, Args... args) = 0;

    /**
     * @brief Visits a PortFieldNode.
     *
     * @param node A pointer to the PortFieldNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the PortFieldNode.
     */
    virtual R visit(const Nodes::PortFieldNode*, Args... args) = 0;

    /**
     * @brief Visits a PortFieldSumNode.
     *
     * @param node A pointer to the PortFieldSumNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the PortFieldSumNode.
     */
    virtual R visit(const Nodes::PortFieldSumNode*, Args... args) = 0;

    /**
     * @brief Visits a TimeShiftNode.
     *
     * @param node A pointer to the TimeShiftNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the TimeShiftNode.
     */
    virtual R visit(const Nodes::TimeShiftNode*, Args... args) = 0;

    /**
     * @brief Visits a TimeIndexNode.
     *
     * @param node A pointer to the TimeIndexNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the TimeIndexNode.
     */
    virtual R visit(const Nodes::TimeIndexNode*, Args... args) = 0;
    /**
     * @brief Visits a TimeSumNode.
     *
     * @param node A pointer to the TimeSumNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the TimeSumNode.
     */
    virtual R visit(const Nodes::TimeSumNode*, Args... args) = 0;
    virtual R visit(const Nodes::TPlusNode*, Args... args) = 0;
    /**
     * @brief Visits a AllTimeSumNode.
     *
     * @param node A pointer to the AllTimeSumNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the AllTimeSumNode.
     */
    virtual R visit(const Nodes::AllTimeSumNode*, Args... args) = 0;

    /**
     * @brief Visits a FunctionNode.
     *
     * @param node A pointer to the FunctionNode to be visited.
     * @param args Additional arguments to be passed to the visitor's methods.
     *
     * @return The result of processing the DualNode.
     */
    virtual R visit(const Nodes::FunctionNode*, Args... args) = 0;

protected:
    std::vector<R> visitChildrenNodes(const Nodes::ParentNode* node);
};

template<class R, class... Args>
std::vector<R> NodeVisitor<R, Args...>::visitChildrenNodes(const Nodes::ParentNode* node)
{
    std::vector<R> result;
    std::ranges::transform(node->getOperands(),
                           std::back_inserter(result),
                           [this](auto& arg) { return dispatch(arg); });
    return result;
}

} // namespace Antares::Expressions::Visitors
