/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include "pi_constantes_externes.h"

#include <yuni/core/math.h>

#define ZERO_POUR_LES_VARIABLES_FIXES 1.e-6

using namespace Yuni;

void OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique(PROBLEME_HEBDO* problemeHebdo,
                                                               int PdtHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    for (int i = 0; i < ProblemeAResoudre->NombreDeVariables; i++)
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[i] = nullptr;

    VALEURS_DE_NTC_ET_RESISTANCES& ValeursDeNTC = problemeHebdo->ValeursDeNTC[PdtHebdo];
    const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[0];

    for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
    {
        int var = CorrespondanceVarNativesVarOptim.NumeroDeVariableDeLInterconnexion[interco];
        ProblemeAResoudre->Xmax[var] = ValeursDeNTC.ValeurDeNTCOrigineVersExtremite[interco];
        ProblemeAResoudre->Xmin[var] = -(ValeursDeNTC.ValeurDeNTCExtremiteVersOrigine[interco]);

        if (ProblemeAResoudre->Xmax[var] - ProblemeAResoudre->Xmin[var]
            < ZERO_POUR_LES_VARIABLES_FIXES)
        {
            ProblemeAResoudre->X[var]
              = 0.5 * (ProblemeAResoudre->Xmax[var] - ProblemeAResoudre->Xmin[var]);
            ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_FIXE;
        }
        else
        {
            if (Math::Infinite(ProblemeAResoudre->Xmax[var]) == 1)
            {
                if (Math::Infinite(ProblemeAResoudre->Xmin[var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_NON_BORNEE;
                else
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_BORNEE_INFERIEUREMENT;
            }
            else
            {
                if (Math::Infinite(ProblemeAResoudre->Xmin[var]) == -1)
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_BORNEE_SUPERIEUREMENT;
                else
                    ProblemeAResoudre->TypeDeVariable[var] = VARIABLE_BORNEE_DES_DEUX_COTES;
            }
        }
        double* adresseDuResultat = &(ValeursDeNTC.ValeurDuFlux[interco]);
        ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
    }
}
