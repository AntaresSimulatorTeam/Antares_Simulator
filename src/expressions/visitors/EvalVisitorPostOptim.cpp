/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/expressions/visitors/EvalVisitorPostOptim.h"

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/visitors/TimeIndexVisitor.h"

namespace Antares::Expressions::Visitors
{

EvalVisitorPostOptim::EvalVisitorPostOptim(const IEvaluationContextProvider& contextProvider,
                                           Optimisation::LinearProblemApi::FillContext fillContext,
                                           const ModelerStudy::SystemModel::Component* component):
    EvalVisitor(contextProvider.provide(*component), // TODO perf: avoid this copy
                fillContext,
                component),
    contextProvider_(contextProvider)
{
}

std::string EvalVisitorPostOptim::name() const
{
    return "EvalVisitorPostOptim";
}

EvaluationResult EvalVisitorPostOptim::visit(const Nodes::PortFieldSumNode* node)
{
    std::string portId = node->getPortName();
    std::string fieldId = node->getFieldName();

    auto idxType = Antares::Expressions::Visitors::TimeIndexVisitor(*component_, contextProvider_)
                     .dispatch(node);
    if (idxType == TimeIndex::CONSTANT_IN_TIME_AND_SCENARIO)
    {
        EvaluationResult result(0.);
        for (const auto connectionEnd: component_->componentConnectionsViaPort(portId))
        {
            auto* component = connectionEnd.component();
            auto* port = connectionEnd.port();
            EvalVisitorPostOptim visitor(contextProvider_, fillContext_, component);
            const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
            result += visitor.dispatch(node);
        }
        return result;
    }

    std::vector<double> results;
    results.resize(fillContext_.getLocalNumberOfTimeSteps());
    for (auto timeStep = fillContext_.getGlobalFirstTimeStep();
         timeStep <= fillContext_.getGlobalLastTimeStep();
         ++timeStep)
    {
        for (const auto connectionEnd: component_->componentConnectionsViaPort(portId))
        {
            auto* component = connectionEnd.component();
            auto* port = connectionEnd.port();
            EvalVisitorPostOptim visitor(contextProvider_, fillContext_, component);
            const Nodes::Node* node = component->nodeAtPortField(port->Id(), fieldId);
            results[timeStep] += visitor.dispatch(node).valueAsDouble();
        }
    }
    return EvaluationResult{results};
}

} // namespace Antares::Expressions::Visitors
