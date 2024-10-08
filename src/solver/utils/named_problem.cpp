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
#include "antares/solver/utils/named_problem.h"

#include <algorithm>

#include "antares/solver/utils/basis_status.h"

namespace Antares::Optimization
{
PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                                                 const std::vector<std::string>& NomDesContraintes,
                                                 const std::vector<bool>& VariablesEntieres,
                                                 bool UseNamedProblems,
                                                 bool SolverLogs):
    NomDesVariables(NomDesVariables),
    NomDesContraintes(NomDesContraintes),
    useNamedProblems_(UseNamedProblems),
    VariablesEntieres(VariablesEntieres)
{
    AffichageDesTraces = SolverLogs ? OUI_SPX : NON_SPX;
}

bool PROBLEME_SIMPLEXE_NOMME::isMIP() const
{
    return std::any_of(VariablesEntieres.cbegin(),
                       VariablesEntieres.cend(),
                       [](bool x) { return x; });
}

bool PROBLEME_SIMPLEXE_NOMME::basisExists() const
{
    return basisStatus.exists();
}

} // namespace Antares::Optimization
