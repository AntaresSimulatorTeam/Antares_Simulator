
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

#include "antares/solver/optimisation/HebdoProblemToLpsTranslator.h"
#include "antares/solver/utils/filename.h"

namespace Antares::Solver
{

HebdoProblemToLpsTranslator::HebdoProblemToLpsTranslator(
  std::shared_ptr<OptPeriodStringGenerator> periodStringGenerator,
  unsigned int optimizationNumber) noexcept :
 periodStringGenerator_(periodStringGenerator)
, optimizationNumber_(optimizationNumber)
{
}

template<class T, class U>
void copy(const T& in, U& out)
{
    std::ranges::copy(in, std::back_inserter(out));
}

HebdoDataFromAntaresPtr HebdoProblemToLpsTranslator::translate(
  const PROBLEME_ANTARES_A_RESOUDRE* problem) const
{
    if (problem == nullptr)
        return {};
    auto ret = std::make_unique<HebdoDataFromAntares>();

    copy(problem->CoutLineaire, ret->CoutLineaire);
    copy(problem->Xmax, ret->Xmax);
    copy(problem->Xmin, ret->Xmin);
    copy(problem->NomDesVariables, ret->variables);
    copy(problem->NomDesContraintes, ret->constraints);
    copy(problem->SecondMembre, ret->SecondMembre);
    copy(problem->Sens, ret->Sens);

    std::string problemName = createMPSfilename(*periodStringGenerator_, optimizationNumber_);
    copy(problemName, ret->name);

    return ret;
}

ConstantDataFromAntaresPtr HebdoProblemToLpsTranslator::commonProblemData(const PROBLEME_ANTARES_A_RESOUDRE* problem) const {
    if (problem == nullptr)
        return nullptr;

    if (problem->NombreDeVariables <= 0) {
        throw std::runtime_error("NombreDeVariables must be strictly positive");
    }
    if (problem->NombreDeContraintes <= 0) {
        throw std::runtime_error("NombreDeContraintes must be strictly positive");
    }

    if (problem->NombreDeContraintes > problem->IndicesDebutDeLigne.size()) {
        throw std::runtime_error("NombreDeContraintes exceed IndicesDebutDeLigne size");
    }

    if (problem->NombreDeContraintes > problem->NombreDeTermesDesLignes.size()) {
        throw std::runtime_error("NombreDeContraintes exceed NombreDeTermesDesLignes size");
    }

    auto ret = std::make_unique<ConstantDataFromAntares>();

    ret->NombreDeVariables = problem->NombreDeVariables;
    ret->NombreDeContraintes = problem->NombreDeContraintes;

    ret->NombreDeCoefficients = problem->IndicesDebutDeLigne[problem->NombreDeContraintes - 1] +
                                problem->NombreDeTermesDesLignes[problem->NombreDeContraintes - 1];

    copy(problem->TypeDeVariable, ret->TypeDeVariable);

    copy(problem->CoefficientsDeLaMatriceDesContraintes,
    ret->CoefficientsDeLaMatriceDesContraintes);
    copy(problem->IndicesColonnes, ret->IndicesColonnes);

    copy(problem->IndicesDebutDeLigne, ret->Mdeb);
    return ret;
}
} // namespace Antares::Solver