/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <antares/logs/logs.h>
#include <antares/solver/optim/ortoolsImpl/mipObjective.h>
#include <antares/solver/optim/ortoolsImpl/mipVariable.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

OrtoolsMipObjective::OrtoolsMipObjective(operations_research::MPObjective* objective):
    objective_(objective)
{
}

void OrtoolsMipObjective::setCoefficient(Api::MipVariable* var, double coefficient)
{
    auto* mpvar = dynamic_cast<OrtoolsMipVariable*>(var);
    if (!mpvar)
    {
        logs.error() << "Invalid cast, tried from Api::MipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }

    objective_->SetCoefficient(mpvar->getMpVar(), coefficient);
}

void OrtoolsMipObjective::setMaximization()
{
    objective_->SetMaximization();
}

void OrtoolsMipObjective::setMinimization()
{
    objective_->SetMinimization();
}

double OrtoolsMipObjective::getCoefficient(Api::MipVariable* var)
{
    auto* mpvar = dynamic_cast<OrtoolsMipVariable*>(var);
    if (!mpvar)
    {
        logs.error() << "Invalid cast, tried from Api::MipVariable to OrtoolsMipVariable";
        throw std::bad_cast();
    }

    return objective_->GetCoefficient(mpvar->getMpVar());
}

bool OrtoolsMipObjective::getMaximization()
{
    return objective_->maximization();
}

bool OrtoolsMipObjective::getMinimization()
{
    return objective_->minimization();
}

operations_research::MPObjective* OrtoolsMipObjective::getMpObjective()
{
    return objective_;
}

} // namespace Antares::Solver::Optim::OrtoolsImpl
