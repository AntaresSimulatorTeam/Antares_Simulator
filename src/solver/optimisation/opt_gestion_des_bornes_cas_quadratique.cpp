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

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "pi_constantes_externes.h"
#include "variables/VariableManagerUtils.h"

#define ZERO_POUR_LES_VARIABLES_FIXES 1.e-6

using namespace Yuni;

void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique(PROBLEME_HEBDO* problemeHebdo,
                                                               int PdtHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
    {
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i] = nullptr;
    }

    VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC = problemeHebdo->ValeursDeNTC[PdtHebdo];

    for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    {
        int var = variableManager.NTCDirect(interco, 0);
        ProblemeAResoudre->Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco];
        ProblemeAResoudre->Xmin[var] = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco]);

        if (ProblemeAResoudre->Xmax[var] - ProblemeAResoudre->Xmin[var]
            < ZERO_POUR_LES_VARIABLES_FIXES)
        {
            ProblemeAResoudre->X[var] = 0.5
                                        * (ProblemeAResoudre->Xmax[var]
                                           - ProblemeAResoudre->Xmin[var]);
            ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_FIXE;
        }
        else
        {
            const double Xmin = ProblemeAResoudre->Xmin[var];
            const double Xmax = ProblemeAResoudre->Xmax[var];
            if (std::isinf(Xmax) && Xmax > 0)
            {
                if (std::isinf(Xmin) && Xmin < 0)
                {
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_NON_BORNEE;
                }
                else
                {
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
                }
            }
            else
            {
                if (std::isinf(Xmin) && Xmin < 0)
                {
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                }
                else
                {
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
                }
            }
        }
        double* adresseDuResultat = &(ValeursDeNTC.ValeurDuFlux[interco]);
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
    }
}
