/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"
#include "opt_fonctions.h"

void OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    std::vector<double> Pi(ProblemeAResoudre->NombreDeVariables, 0.);
    std::vector<int> Colonne(ProblemeAResoudre->NombreDeVariables, 0);

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    const CORRESPONDANCES_DES_VARIABLES& correspondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[0];

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays - 1; pays++)
    {
        int nombreDeTermes = 0;

        int interco = problemeHebdo->IndexDebutIntercoOrigine[pays];
        while (interco >= 0)
        {
            if (int var
                = correspondanceVarNativesVarOptim.NumeroDeVariableDeLInterconnexion[interco];
                var >= 0)
            {
                Pi[nombreDeTermes] = 1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }
            interco = problemeHebdo->IndexSuivantIntercoOrigine[interco];
        }
        interco = problemeHebdo->IndexDebutIntercoExtremite[pays];
        while (interco >= 0)
        {
            if (int var
                = correspondanceVarNativesVarOptim.NumeroDeVariableDeLInterconnexion[interco];
                var >= 0)
            {
                Pi[nombreDeTermes] = -1.0;
                Colonne[nombreDeTermes] = var;
                nombreDeTermes++;
            }
            interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
        }

        problemeHebdo->NumeroDeContrainteDeSoldeDEchange[pays]
          = ProblemeAResoudre->NombreDeContraintes;

        OPT_ChargerLaContrainteDansLaMatriceDesContraintes(
          ProblemeAResoudre, Pi, Colonne, nombreDeTermes, '=');
    }
}
