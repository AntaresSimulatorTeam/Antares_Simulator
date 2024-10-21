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
PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre,
                                                 bool UseNamedProblems,
                                                 bool SolverLogs):
    NomDesVariables(problemeAResoudre->NomDesVariables),
    NomDesContraintes(problemeAResoudre->NomDesContraintes),
    VariablesEntieres(problemeAResoudre->VariablesEntieres),
    useNamedProblems_(UseNamedProblems),
    basisStatus(basisStatus)
{
    AffichageDesTraces = SolverLogs ? OUI_SPX : NON_SPX;
    NombreMaxDIterations = -1;
    DureeMaxDuCalcul = -1.;
    CoutLineaire = problemeAResoudre->CoutLineaire.data();
    X = problemeAResoudre->X.data();
    Xmin = problemeAResoudre->Xmin.data();
    Xmax = problemeAResoudre->Xmax.data();
    NombreDeVariables = problemeAResoudre->NombreDeVariables;
    TypeDeVariable = problemeAResoudre->TypeDeVariable.data();

    NombreDeContraintes = problemeAResoudre->NombreDeContraintes;
    IndicesDebutDeLigne = problemeAResoudre->IndicesDebutDeLigne.data();
    NombreDeTermesDesLignes = problemeAResoudre->NombreDeTermesDesLignes.data();
    IndicesColonnes = problemeAResoudre->IndicesColonnes.data();
    CoefficientsDeLaMatriceDesContraintes = problemeAResoudre
                                                       ->CoefficientsDeLaMatriceDesContraintes
                                                       .data();
    Sens = problemeAResoudre->Sens.data();
    SecondMembre = problemeAResoudre->SecondMembre.data();
    ChoixDeLAlgorithme = SPX_DUAL;
    TypeDePricing = PRICING_STEEPEST_EDGE;
    StrategieAntiDegenerescence = AGRESSIF;
    PositionDeLaVariable = problemeAResoudre->PositionDeLaVariable.data();
    NbVarDeBaseComplementaires = 0;
    ComplementDeLaBase = problemeAResoudre->ComplementDeLaBase.data();
    LibererMemoireALaFin = NON_SPX;
    UtiliserCoutMax = NON_SPX;
    CoutMax = 0.0;
    CoutsMarginauxDesContraintes = problemeAResoudre->CoutsMarginauxDesContraintes.data();
    CoutsReduits = problemeAResoudre->CoutsReduits.data();
    NombreDeContraintesCoupes = 0;
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
