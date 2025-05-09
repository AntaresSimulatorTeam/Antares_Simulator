// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#include "antares/solver/simulation/sim_binding_constraints_rhs.h"

#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

static void setRHSforHourlyBC()
{
}

static void setRHSforDailyBC()
{
}

static void setRHSforWeeklyBC()
{
}

namespace Simulation
{
void setBindingConstraintsRHS(PROBLEME_HEBDO& problem,
                              const Antares::Data::BindingConstraintsRepository& bindingConstraints,
                              const Antares::Data::BindingConstraintGroupRepository& bcGroups,
                              const unsigned PasDeTempsDebut,
                              const unsigned NombreDePasDeTemps,
                              const unsigned weekFirstDay)
{
    unsigned bcIndex = 0;
    for (const auto& bc: bindingConstraints.activeConstraints())
    {
        switch (bc->type())
        {
        case Data::BindingConstraint::typeHourly:
        {
            setRHSforHourlyBC();
        }
        case Data::BindingConstraint::typeDaily:
        {
            setRHSforDailyBC();
        }
        case Data::BindingConstraint::typeWeekly:
        {
            setRHSforWeeklyBC();
        }
        case Data::BindingConstraint::typeUnknown:
        case Data::BindingConstraint::typeMax:
        default:
        {
            logs.error() << "internal error. Please submit a full bug report";
            assert(false && "invalid constraint type");
            break;
        }
        }
        bcIndex++;
    }
}
} // namespace Simulation
