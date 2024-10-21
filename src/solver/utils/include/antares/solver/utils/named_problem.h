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

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares
{
namespace Optimization
{
class BasisStatus;

struct PROBLEME_SIMPLEXE_NOMME: public PROBLEME_SIMPLEXE
{
public:
    PROBLEME_SIMPLEXE_NOMME(PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre,
                            bool UseNamedProblems,
                            bool SolverLogs);

private:
    const std::vector<std::string>& NomDesVariables;
    const std::vector<std::string>& NomDesContraintes;
    bool useNamedProblems_;

public:
    const std::vector<bool>& VariablesEntieres;
    BasisStatus& basisStatus;

    bool isMIP() const;
    bool basisExists() const;

    bool UseNamedProblems() const
    {
        return useNamedProblems_;
    }

    void SetUseNamedProblems(bool useNamedProblems)
    {
        useNamedProblems_ = useNamedProblems;
    }

    const std::vector<std::string>& VariableNames() const
    {
        return NomDesVariables;
    }

    const std::vector<std::string>& ConstraintNames() const
    {
        return NomDesContraintes;
    }

    bool IntegerVariable(size_t idx) const
    {
        return VariablesEntieres[idx];
    }
};
} // namespace Optimization
} // namespace Antares
