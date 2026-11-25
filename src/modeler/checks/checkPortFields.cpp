// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <fmt/format.h>
#include <string>
#include <vector>

#include <antares/expressions/iterators/pre-order.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/logs/logs.h>
#include <antares/solver/modeler/checks/checkPortFields.h>

using namespace Antares::Expressions::Nodes;
using namespace Antares::ModelerStudy::SystemModel;

namespace Antares::Solver::Checks
{
namespace
{
void collectErrors(const Model& model,
                   const Expression& expression,
                   std::vector<std::string>& errors)
{
    for (const auto& node: ASTconst(expression.RootNode()))
    {
        if (const auto* portField = dynamic_cast<const PortFieldNode*>(&node))
        {
            errors.push_back(fmt::format(
              "Model '{}': In expression '{}': Port field '{}.{}' cannot be used directly. "
              "Use 'sum_connections({}.{})' to aggregate values from connected components.",
              model.Id(),
              expression.Value(),
              portField->getPortName(),
              portField->getFieldName(),
              portField->getPortName(),
              portField->getFieldName()));
        }
    }
}

void collectModelErrors(const Model& model, std::vector<std::string>& errors)
{
    for (const auto& constraint: model.Constraints())
    {
        collectErrors(model, constraint.expression(), errors);
    }
    for (const auto& objective: model.Objectives())
    {
        collectErrors(model, objective.expression(), errors);
    }
    for (const auto& [_, output]: model.ExtraOutputs())
    {
        collectErrors(model, output.expression(), errors);
    }
    for (const auto& [_, definition]: model.PortFieldDefinitions())
    {
        collectErrors(model, definition.Definition(), errors);
    }
}

void reportErrors(const std::vector<std::string>& errors)
{
    if (errors.empty())
    {
        return;
    }

    std::string combined;
    for (const auto& error: errors)
    {
        logs.error() << error;
        if (!combined.empty())
        {
            combined += '\n';
        }
        combined += error;
    }
    throw PortFieldError(combined);
}
} // namespace

void checkModelPortFields(const Model& model)
{
    std::vector<std::string> errors;
    collectModelErrors(model, errors);
    reportErrors(errors);
}

void checkPortFields(const ModelerData& data)
{
    std::vector<std::string> errors;
    for (const auto& library: data.libraries)
    {
        for (const auto& [_, model]: library.Models())
        {
            collectModelErrors(model, errors);
        }
    }
    reportErrors(errors);
}
} // namespace Antares::Solver::Checks
