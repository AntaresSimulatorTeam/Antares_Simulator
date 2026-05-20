// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <ExprVisitor.h>

#include <boost/algorithm/string.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/io/inputs/model-converter/convertorVisitor.h>
#include "antares/expressions/nodes/TimeSumNode.h"

#include "ExprLexer.h"
#include "ExprParser.h"

namespace Antares::IO::Inputs::ModelConverter
{

using namespace Antares::Expressions::Nodes;
using namespace Antares::Optimisation;

/// Visitor to convert ANTLR expressions to Antares::Expressions::Nodes
class ConvertorVisitor final: public ExprVisitor
{
public:
    ConvertorVisitor(Expressions::Registry<Node>& registry, const YmlModel::Model& model);

    std::any visit(antlr4::tree::ParseTree* tree) override;

    Node* convertIdentifier(const std::string& identifier) const; // gp : should be private
    std::any visitIdentifier(ExprParser::IdentifierContext* context) override;
    std::any visitMuldiv(ExprParser::MuldivContext* context) override;
    std::any visitFullexpr(ExprParser::FullexprContext* context) override;
    std::any visitShift(ExprParser::ShiftContext* context) override;
    std::any visitNegation(ExprParser::NegationContext* context) override;
    std::any visitExpression(ExprParser::ExpressionContext* context) override;
    std::any visitComparison(ExprParser::ComparisonContext* context) override;
    std::any visitAddsub(ExprParser::AddsubContext* context) override;
    std::any visitPortField(ExprParser::PortFieldContext* context) override;
    std::any visitNumber(ExprParser::NumberContext* context) override;
    std::any visitTimeIndex(ExprParser::TimeIndexContext* context) override;
    std::any visitTimeShift(ExprParser::TimeShiftContext* context) override;
    std::any visitFunction(ExprParser::FunctionContext* context) override;
    std::any visitArgList(ExprParser::ArgListContext* context) override;
    std::any visitTimeSum(ExprParser::TimeSumContext* context) override;
    std::any visitAllTimeSum(ExprParser::AllTimeSumContext* context) override;
    std::any visitSignedAtom(ExprParser::SignedAtomContext* context) override;
    std::any visitUnsignedAtom(ExprParser::UnsignedAtomContext* context) override;
    std::any visitRightAtom(ExprParser::RightAtomContext* context) override;
    std::any visitSignedExpression(ExprParser::SignedExpressionContext* context) override;
    std::any visitShiftAddsub(ExprParser::ShiftAddsubContext* context) override;
    std::any visitShiftMuldiv(ExprParser::ShiftMuldivContext* context) override;
    std::any visitRightMuldiv(ExprParser::RightMuldivContext* context) override;
    std::any visitRightExpression(ExprParser::RightExpressionContext* context) override;
    std::any visitTimeShiftExpr(ExprParser::TimeShiftExprContext* context) override;
    std::any visitTimeIndexExpr(ExprParser::TimeIndexExprContext* context) override;
    std::any visitPortFieldExpr(ExprParser::PortFieldExprContext* context) override;
    std::any visitPortFieldSum(ExprParser::PortFieldSumContext* context) override;
    std::any visitPower(ExprParser::PowerContext* context) override;
    std::any visitRightPower(ExprParser::RightPowerContext* context) override;
    std::any visitShiftPower(ExprParser::ShiftPowerContext* context) override;
    std::any visitSum_bound(ExprParser::Sum_boundContext* context) override;

private:
    // Methods
    // -------
    std::any visitDual(ExprParser::ArgListContext* context);
    std::any visitReducedCost(ExprParser::ArgListContext* context);
    std::any visitMax(ExprParser::ArgListContext* context);
    std::any visitMin(ExprParser::ArgListContext* arglist);
    std::any visitFloor(ExprParser::ArgListContext* context);
    std::any visitCeil(ExprParser::ArgListContext* context);
    Node* extractOneArgument(ExprParser::ArgListContext* context, const std::string& opName);
    std::any buildShiftNode(Node* shifted_expr, ExprParser::ShiftContext* context);
    Node* NodeFromShiftContext(ExprParser::Shift_exprContext* shift_expr);
    PortFieldNode* processPortRule(ExprParser::PortFieldExprContext* context);

    template<class T>
    std::any ProcessChildren(const std::vector<T*>& exprContexts);

    template<class T>
    std::any processPower(const std::vector<T*>& exprContexts);

    // Data members
    // ------------
    Expressions::Registry<Node>& registry_;
    const YmlModel::Model& model_;
};

class AntaresErrorListener: public antlr4::BaseErrorListener
{
public:
    void syntaxError(antlr4::Recognizer*,
                     antlr4::Token*,
                     size_t,
                     size_t,
                     const std::string& msg,
                     std::exception_ptr) override
    {
        errors.push_back(msg);
    }

    void checkErrors(const std::string& expr) const
    {
        if (errors.empty())
        {
            return;
        }

        std::string fullMsg = "Error(s) while parsing expression: '" + expr + "'\n";
        for (const auto& err: errors)
        {
            fullMsg += "  - " + err + "\n";
        }
        throw AntlrParsingError(fullMsg);
    }

private:
    std::vector<std::string> errors;
};

Expressions::NodeRegistry convertExpressionToNode(const std::string& exprStr,
                                                  const YmlModel::Model& model)
{
    if (exprStr.empty())
    {
        return {};
    }

    // ANTLR setup
    antlr4::ANTLRInputStream input(exprStr);
    ExprLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    ExprParser parser(&tokens);

    // error handling
    AntaresErrorListener errorListener;
    lexer.removeErrorListeners();  // remove antlr logs to keep ours
    parser.removeErrorListeners(); // same
    lexer.addErrorListener(&errorListener);
    parser.addErrorListener(&errorListener);

    // actual parsing
    ExprParser::FullexprContext* tree = parser.fullexpr();
    errorListener.checkErrors(exprStr);

    // tree conversion
    Expressions::Registry<Node> registry;
    ConvertorVisitor visitor(registry, model);
    auto root = std::any_cast<Node*>(visitor.visit(tree));
    return Expressions::NodeRegistry(root, std::move(registry));
}

ConvertorVisitor::ConvertorVisitor(Expressions::Registry<Node>& registry,
                                   const YmlModel::Model& model):
    registry_(registry),
    model_(model)
{
}

std::any ConvertorVisitor::visit(antlr4::tree::ParseTree* tree)
{
    return tree->accept(this);
}

Node* ConvertorVisitor::convertIdentifier(const std::string& identifier) const
{
    for (const auto& param: model_.parameters)
    {
        if (param.id == identifier)
        {
            auto v = variability(param.time_dependent, param.scenario_dependent);
            return static_cast<Node*>(registry_.create<ParameterNode>(param.id, v));
        }
    }

    const auto& variables = model_.variables;
    for (std::size_t index = 0; index < variables.size(); ++index)
    {
        const auto& var = variables[index];
        if (var.id == identifier)
        {
            auto v = variability(var.time_dependent, var.scenario_dependent);
            return static_cast<Node*>(registry_.create<VariableNode>(var.id, index, v));
        }
    }
    throw InputError("No parameter or variable found for this identifier: " + identifier);
}

std::any ConvertorVisitor::visitIdentifier(ExprParser::IdentifierContext* context)
{
    return convertIdentifier(context->getText());
}

std::any ConvertorVisitor::visitMuldiv(ExprParser::MuldivContext* context)
{
    auto* left = std::any_cast<Node*>(visit(context->expr(0)));
    auto* right = std::any_cast<Node*>(visit(context->expr(1)));

    std::string op = context->op->getText();
    return (op == "*") ? static_cast<Node*>(registry_.create<MultiplicationNode>(left, right))
                       : static_cast<Node*>(registry_.create<DivisionNode>(left, right));
}

std::any ConvertorVisitor::visitFullexpr(ExprParser::FullexprContext* context)
{
    return context->expr()->accept(this);
}

std::any ConvertorVisitor::visitNegation(ExprParser::NegationContext* context)
{
    Node* n = std::any_cast<Node*>(context->expr()->accept(this));
    return static_cast<Node*>(registry_.create<NegationNode>(n));
}

std::any ConvertorVisitor::visitExpression(ExprParser::ExpressionContext* context)
{
    return context->expr()->accept(this);
}

// TPlus(expr) <= borne mobile
// expr <= borne fixe

// UnaryNode

// from="t+1" => TPlus(Literal(1))
//   from="1" => Literal(1)
std::any ConvertorVisitor::visitSum_bound(ExprParser::Sum_boundContext* context)
{
    if (auto* shift = context->shift())
    {
        auto* offset = std::any_cast<Node*>(shift->accept(this));
        return static_cast<Node*>(registry_.create<TPlusNode>(offset));
    }
    if (auto* expr = context->expr())
    {
        return expr->accept(this);
    }
    throw InputError("Invalid sum bound");
}

std::any ConvertorVisitor::visitComparison(ExprParser::ComparisonContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->expr(0)));
    Node* right = std::any_cast<Node*>(visit(context->expr(1)));

    std::string op = context->COMPARISON()->getText();
    if (op == "=")
    {
        return static_cast<Node*>(registry_.create<EqualNode>(left, right));
    }
    else if (op == "<=")
    {
        return static_cast<Node*>(registry_.create<LessThanOrEqualNode>(left, right));
    }
    else
    {
        return static_cast<Node*>(registry_.create<GreaterThanOrEqualNode>(left, right));
    }
}

void extractSumOperands(Node* node, std::vector<Node*>& operands)
{
    if (auto* sumNode = dynamic_cast<SumNode*>(node))
    {
        for (auto* operand: sumNode->getOperands())
        {
            extractSumOperands(operand, operands);
        }
    }
    else
    {
        operands.emplace_back(node);
    }
}

std::any ConvertorVisitor::visitAddsub(ExprParser::AddsubContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->expr(0)));
    Node* right = std::any_cast<Node*>(visit(context->expr(1)));

    std::string op = context->op->getText();
    if (op == "-")
    {
        return static_cast<Node*>(registry_.create<SubtractionNode>(left, right));
    }
    // Sum node, flatten all operands in one vector if possible
    std::vector<Node*> operands;
    extractSumOperands(left, operands);
    extractSumOperands(right, operands);
    return static_cast<Node*>(registry_.create<SumNode>(operands));
}

static bool isPortRegistered(const std::string& portId, const std::vector<YmlModel::Port>& ports)
{
    for (const auto& [id, _]: ports)
    {
        if (id == portId)
        {
            return true;
        }
    }
    return false;
}

PortFieldNode* ConvertorVisitor::processPortRule(ExprParser::PortFieldExprContext* context)
{
    const auto [portId, portField] = std::any_cast<std::pair<std::string, std::string>>(
      context->accept(this));

    if (isPortRegistered(portId, model_.ports))
    {
        return registry_.create<PortFieldNode>(portId, portField);
    }
    throw IO::Inputs::InputError("No port found for this identifier: " + portId);
}

std::any ConvertorVisitor::visitPortField(ExprParser::PortFieldContext* context)
{
    return static_cast<Node*>(processPortRule(context->portFieldExpr()));
}

std::any ConvertorVisitor::visitNumber(ExprParser::NumberContext* context)
{
    double d = std::stod(context->getText());
    return static_cast<Node*>(registry_.create<LiteralNode>(d));
}

std::any ConvertorVisitor::visitTimeIndex(ExprParser::TimeIndexContext* context)
{
    Node* expr = convertIdentifier(context->IDENTIFIER()->getText());
    auto* index = registry_.create<LiteralNode>(std::stoi(context->expr()->getText()));
    return static_cast<Node*>(registry_.create<TimeIndexNode>(expr, index));
}

std::any ConvertorVisitor::buildShiftNode(Node* shifted_expr, ExprParser::ShiftContext* context)
{
    if (auto shift_maybe = context->shift_expr()) // [t+1], [t-1], etc.
    {
        Node* shift = std::any_cast<Node*>(shift_maybe->accept(this));
        return static_cast<Node*>(registry_.create<TimeShiftNode>(shifted_expr, shift));
    }
    else // [t]
    {
        return shifted_expr; // implicit conversion to std::any
    }
}

std::any ConvertorVisitor::visitTimeShift(ExprParser::TimeShiftContext* context)
{
    return buildShiftNode(convertIdentifier(context->IDENTIFIER()->getText()), context->shift());
}

std::any ConvertorVisitor::visitTimeShiftExpr(ExprParser::TimeShiftExprContext* context)
{
    return buildShiftNode(std::any_cast<Node*>(context->expr()->accept(this)), context->shift());
}

std::any ConvertorVisitor::visitTimeIndexExpr(ExprParser::TimeIndexExprContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->expr(0)));
    Node* right = std::any_cast<Node*>(visit(context->expr(1)));
    return static_cast<Node*>(registry_.create<TimeIndexNode>(left, right));
}

std::any ConvertorVisitor::visitPortFieldExpr(ExprParser::PortFieldExprContext* context)
{
    return std::make_pair(context->IDENTIFIER()[0]->getText(), context->IDENTIFIER()[1]->getText());
}

std::any ConvertorVisitor::visitPortFieldSum(ExprParser::PortFieldSumContext* context)
{
    const auto port = processPortRule(context->portFieldExpr());
    const auto portName = port->getPortName();
    const auto fieldName = port->getFieldName();

    return static_cast<Node*>(registry_.create<PortFieldSumNode>(portName, fieldName));
}

std::vector<std::string> ExpressionsToIds(const std::vector<ExprParser::ExprContext*>& expressions)
{
    std::vector<std::string> ids;
    auto expressionToId = [](auto* expression) { return expression->getText(); };
    std::ranges::transform(expressions, std::back_inserter(ids), expressionToId);
    return ids;
}

std::any ConvertorVisitor::visitDual(ExprParser::ArgListContext* context)
{
    std::vector<std::string> argIds = ExpressionsToIds(context->expr());

    if (argIds.size() != 1)
    {
        throw InputError("dual operator expects exactly one constraint id got: "
                         + boost::algorithm::join(argIds, ", "));
    }

    const std::string constraint_id = argIds[0];

    unsigned index = 0;
    const auto search_constraint = [&](const auto& constraints) -> Node*
    {
        for (const auto& c: constraints)
        {
            if (c.id == constraint_id)
            {
                auto* constraintIdNode = registry_.create<ParameterNode>(constraint_id);
                auto* constraintIndex = registry_.create<LiteralNode>(index);
                return static_cast<Node*>(registry_.create<FunctionNode>(FunctionNodeType::dual,
                                                                         constraintIdNode,
                                                                         constraintIndex));
            }
            ++index;
        }
        return nullptr;
    };

    if (Node* node = search_constraint(model_.constraints))
    {
        return node;
    }
    if (Node* node = search_constraint(model_.binding_constraints))
    {
        return node;
    }

    throw InputError("dual called with unknown constraint '" + constraint_id + "' in model '"
                     + model_.id + "'");
}

std::any ConvertorVisitor::visitReducedCost(ExprParser::ArgListContext* context)
{
    std::vector<std::string> argIds = ExpressionsToIds(context->expr());

    if (argIds.size() != 1)
    {
        throw InputError("reduced_cost operator expects exactly one variable id got: "
                         + boost::algorithm::join(argIds, ", "));
    }

    const std::string var_id = argIds[0];

    unsigned index = 0;
    for (const auto& var: model_.variables)
    {
        if (var.id == var_id)
        {
            auto* varNode = registry_.create<VariableNode>(var_id, index);
            return static_cast<Node*>(
              registry_.create<FunctionNode>(FunctionNodeType::reduced_cost, varNode));
        }
        ++index;
    }
    throw InputError("reduced_cost called with unknown variable '" + var_id + "' in model '"
                     + model_.id + "'");
}

std::any ConvertorVisitor::visitMax(ExprParser::ArgListContext* context)
{
    const auto nodes = std::any_cast<std::vector<Node*>>(context->accept(this));
    if (nodes.size() < 2)
    {
        throw InputError("max operator expects at least 2 operands got "
                         + std::to_string(nodes.size()));
    }
    return static_cast<Node*>(registry_.create<FunctionNode>(FunctionNodeType::max, nodes));
}

std::any ConvertorVisitor::visitMin(ExprParser::ArgListContext* context)
{
    const auto nodes = std::any_cast<std::vector<Node*>>(context->accept(this));
    if (nodes.size() < 2)
    {
        throw InputError("min operator expects at least 2 operands got "
                         + std::to_string(nodes.size()));
    }
    return static_cast<Node*>(registry_.create<FunctionNode>(FunctionNodeType::min, nodes));
}

Node* ConvertorVisitor::extractOneArgument(ExprParser::ArgListContext* context,
                                           const std::string& opName)
{
    const auto nodes = std::any_cast<std::vector<Node*>>(context->accept(this));
    if (size_t size = nodes.size(); size > 1)
    {
        std::string err_msg = opName + "() expects 1 argument, but has " + std::to_string(size);
        throw IO::Inputs::InputError(err_msg);
    }
    return nodes[0];
}

std::any ConvertorVisitor::visitFloor(ExprParser::ArgListContext* context)
{
    Node* node = extractOneArgument(context, "floor");
    return static_cast<Node*>(registry_.create<FunctionNode>(FunctionNodeType::floor, node));
}

std::any ConvertorVisitor::visitCeil(ExprParser::ArgListContext* context)
{
    Node* node = extractOneArgument(context, "ceil");
    return static_cast<Node*>(registry_.create<FunctionNode>(FunctionNodeType::ceil, node));
}

std::any ConvertorVisitor::visitFunction(ExprParser::FunctionContext* context)
{
    const auto functionName = context->IDENTIFIER()->getText();
    auto* arglist = context->argList();

    if (!arglist || arglist->expr().empty())
    {
        std::string err_msg = functionName + " operator expects an argument, got nothing";
        throw IO::Inputs::InputError(err_msg);
    }

    if (functionName == "reduced_cost")
    {
        return visitReducedCost(arglist);
    }
    else if (functionName == "dual")
    {
        return visitDual(arglist);
    }
    else if (functionName == "max")
    {
        return visitMax(arglist);
    }
    else if (functionName == "min")
    {
        return visitMin(arglist);
    }
    else if (functionName == "floor")
    {
        return visitFloor(arglist);
    }
    else if (functionName == "ceil")
    {
        return visitCeil(arglist);
    }

    throw IO::Inputs::InputError("Invalid function: '" + functionName + "'");
}

template<class T>
std::any ConvertorVisitor::ProcessChildren(const std::vector<T*>& exprContexts)
{
    std::vector<Node*> nodes(exprContexts.size());
    for (unsigned int i = 0; i < exprContexts.size(); ++i)
    {
        auto* expr = exprContexts.at(i);
        nodes[i] = std::any_cast<Node*>(expr->accept(this));
    }
    return nodes;
}

std::any ConvertorVisitor::visitArgList(ExprParser::ArgListContext* context)
{
    const auto exprContexts = context->expr();
    return ProcessChildren(exprContexts);
}

Node* ConvertorVisitor::NodeFromShiftContext(ExprParser::Shift_exprContext* shift_expr)
{
    if (shift_expr)
    {
        return std::any_cast<Node*>(shift_expr->accept(this));
    }
    else
    {
        return registry_.create<LiteralNode>(0);
    }
}

std::any ConvertorVisitor::visitTimeSum(ExprParser::TimeSumContext* context)
{
    auto* from = std::any_cast<Node*>(context->from->accept(this));

    auto* to = std::any_cast<Node*>(context->to->accept(this));

    auto* expr = std::any_cast<Node*>(context->expr()->accept(this));

    return static_cast<Node*>(registry_.create<TimeSumNode>(from, to, expr));
}

std::any ConvertorVisitor::visitAllTimeSum(ExprParser::AllTimeSumContext* context)
{
    auto expr = std::any_cast<Node*>(context->expr()->accept(this));
    return static_cast<Node*>(registry_.create<AllTimeSumNode>(expr));
}

// shift related, not tested
std::any ConvertorVisitor::visitSignedAtom(ExprParser::SignedAtomContext* context)
{
    auto a = context->atom()->accept(this);
    if (context->op->getText() == "-")
    {
        return static_cast<Node*>(registry_.create<NegationNode>(std::any_cast<Node*>(a)));
    }
    return a;
}

std::any ConvertorVisitor::visitUnsignedAtom(ExprParser::UnsignedAtomContext* context)
{
    return context->atom()->accept(this);
}

std::any ConvertorVisitor::visitRightAtom(ExprParser::RightAtomContext* context)
{
    return context->atom()->accept(this);
}

std::any ConvertorVisitor::visitShift(ExprParser::ShiftContext* context)
{
    return NodeFromShiftContext(context->shift_expr());
}

std::any ConvertorVisitor::visitShiftAddsub(ExprParser::ShiftAddsubContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->shift_expr()));
    Node* right = std::any_cast<Node*>(visit(context->right_expr()));
    const auto op = context->op->getText();
    return (op == "+") ? static_cast<Node*>(registry_.create<SumNode>(left, right))
                       : static_cast<Node*>(registry_.create<SubtractionNode>(left, right));
}

std::any ConvertorVisitor::visitShiftMuldiv(ExprParser::ShiftMuldivContext* context)
{
    Node* left = std::any_cast<Node*>(visit(context->shift_expr()));
    Node* right = std::any_cast<Node*>(visit(context->right_expr()));
    const auto op = context->op->getText();
    return (op == "*") ? static_cast<Node*>(registry_.create<MultiplicationNode>(left, right))
                       : static_cast<Node*>(registry_.create<DivisionNode>(left, right));
}

// TODO implement this
std::any ConvertorVisitor::visitRightMuldiv(
  [[maybe_unused]] ExprParser::RightMuldivContext* context)
{
    throw IO::Inputs::InputError("Node right mul div not implemented yet");
}

std::any ConvertorVisitor::visitSignedExpression(ExprParser::SignedExpressionContext* context)
{
    auto a = context->expr()->accept(this);
    if (context->op->getText() == "-")
    {
        return static_cast<Node*>(registry_.create<NegationNode>(std::any_cast<Node*>(a)));
    }
    return a;
}

std::any ConvertorVisitor::visitRightExpression(ExprParser::RightExpressionContext* context)
{
    return context->expr()->accept(this);
}

template<class T>
std::any ConvertorVisitor::processPower(const std::vector<T*>& exprContexts)
{
    const auto powerExpr = std::any_cast<std::vector<Node*>>(ProcessChildren(exprContexts));
    return static_cast<Node*>(
      registry_.create<FunctionNode>(FunctionNodeType::pow, powerExpr.at(0), powerExpr.at(1)));
}

std::any ConvertorVisitor::visitPower(ExprParser::PowerContext* context)
{
    auto exprContexts = context->expr();
    return processPower(exprContexts);
}

std::any ConvertorVisitor::visitRightPower(ExprParser::RightPowerContext* context)
{
    auto exprContexts = context->right_expr();
    return processPower(exprContexts);
}

std::any ConvertorVisitor::visitShiftPower(ExprParser::ShiftPowerContext* context)
{
    auto base = std::any_cast<Node*>(context->shift_expr()->accept(this));
    auto exponent = std::any_cast<Node*>(context->right_expr()->accept(this));
    return static_cast<Node*>(
      registry_.create<FunctionNode>(FunctionNodeType::pow, base, exponent));
}

} // namespace Antares::IO::Inputs::ModelConverter
