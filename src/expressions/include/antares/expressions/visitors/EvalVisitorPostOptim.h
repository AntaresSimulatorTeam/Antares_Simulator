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
#pragma once

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include "antares/expressions/IEvaluationContextProvider.h"
#include "antares/expressions/visitors/EvalVisitor.h"
#include "antares/study/system-model/component.h"

namespace Antares::Expressions::Visitors
{

/**
 * @brief Represents a visitor for evaluating expressions within a given context.
 */
class EvalVisitorPostOptim: public EvalVisitor
{
public:
    /**
     * @brief Constructs an evaluation visitor with the specified context.
     *
     * @param context The evaluation context.
     * @param fillContext
     */
    explicit EvalVisitorPostOptim(const IEvaluationContextProvider& contextProvider,
                                  Optimisation::LinearProblemApi::FillContext fillContext,
                                  const ModelerStudy::SystemModel::Component* component);

    std::string name() const override;

    EvaluationResult visit(const Nodes::PortFieldSumNode* node) override;
    const IEvaluationContextProvider& contextProvider_;
};
} // namespace Antares::Expressions::Visitors
