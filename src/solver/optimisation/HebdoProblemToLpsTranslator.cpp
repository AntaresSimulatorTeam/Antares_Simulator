
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

namespace Antares::Solver
{

template<class T, class U>
void copy(const T& in, U& out) {
    std::ranges::copy(in,
                      std::back_inserter(out));
}

HebdoDataFromAntaresPtr HebdoProblemToLpsTranslator::translate(
  const PROBLEME_ANTARES_A_RESOUDRE* problem) const
{
    if (problem == nullptr)
        return {};
    auto ret = std::make_unique<HebdoDataFromAntares>();
//    LpsFromAntares lps;
//    int const year = hebdoProblem->year + 1;
//    int const week = hebdoProblem->weekInTheYear + 1;
//    int const n = 1; //optimizationNumber;
//    int nvars = hebdoProblem->ProblemeAResoudre->NombreDeVariables;
//    int ncons = hebdoProblem->ProblemeAResoudre->NombreDeContraintes;
//    int neles = hebdoProblem->ProblemeAResoudre->IndicesDebutDeLigne[ncons - 1] + hebdoProblem->ProblemeAResoudre->NombreDeTermesDesLignes[ncons - 1];
//
//    //LpFromAntaresPtr lp(new LpFromAntares);
//    if (week == 1 && n == 1) {
//        Solver::ConstantDataFromAntaresPtr year_ptr(new Solver::ConstantDataFromAntares);
//        year_ptr->NombreDeVariables = nvars;
//        year_ptr->NombreDeCoefficients = neles;
//        year_ptr->NombreDeContraintes = ncons;
//
//        copy(hebdoProblem->ProblemeAResoudre->TypeDeVariable, year_ptr->TypeDeVariable);
//
//        copy(hebdoProblem->ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes, year_ptr->CoefficientsDeLaMatriceDesContraintes);
//        copy(hebdoProblem->ProblemeAResoudre->IndicesColonnes, year_ptr->IndicesColonnes);
//
//        copy(hebdoProblem->ProblemeAResoudre->IndicesDebutDeLigne, year_ptr->Mdeb);
//
//        lps._constant = year_ptr;
//    }
//
//    if (n == 1) //Export only the first optimisation
//    {
//        Solver::HebdoDataFromAntaresPtr week_ptr(new Solver::HebdoDataFromAntares);
//
        copy(problem->CoutLineaire, ret->CoutLineaire);
        copy(problem->Xmax, ret->Xmax);
        copy(problem->Xmin, ret->Xmin);
        copy(problem->NomDesVariables, ret->variables);
        copy(problem->NomDesContraintes, ret->constraints);
        copy(problem->SecondMembre, ret->SecondMembre);
//        copy(hebdoProblem->ProblemeAResoudre->Sens, week_ptr->Sens);
//        //std::string problemName = createMPSfilename(*optPeriodStringGenerator, optimizationNumber);
//        //copy(problemName, week_ptr->name);
//
//        lps._hebdo[{static_cast<unsigned int>(year), static_cast<unsigned int>(week)}] = week_ptr;
//    }
        return ret;
}
} // namespace Solver