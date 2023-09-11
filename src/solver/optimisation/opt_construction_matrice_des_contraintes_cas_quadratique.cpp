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
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"
#include "opt_fonctions.h"
#include "constraint_builder.h"
class ExchangeBalance : private Constraint
{
public:
    using Constraint::Constraint;
    void add(uint32_t pays)
    {
        builder.updateHourWithinWeek(0);

        int interco = problemeHebdo->IndexDebutIntercoOrigine[pays];
        while (interco >= 0)
        {
            builder.include(Variable::IntercoDirectCost(interco), 1.0);
            interco = problemeHebdo->IndexSuivantIntercoOrigine[interco];
        }
        interco = problemeHebdo->IndexDebutIntercoExtremite[pays];
        while (interco >= 0)
        {
            builder.include(Variable::IntercoDirectCost(interco), -1.0);

            interco = problemeHebdo->IndexSuivantIntercoExtremite[interco];
        }

        problemeHebdo->NumeroDeContrainteDeSoldeDEchange[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        builder.build();
    }
};

void OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    std::vector<double> Pi(ProblemeAResoudre->NombreDeVariables, 0.);
    std::vector<int> Colonne(ProblemeAResoudre->NombreDeVariables, 0);

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    const CORRESPONDANCES_DES_VARIABLES& correspondanceVarNativesVarOptim
      = problemeHebdo->CorrespondanceVarNativesVarOptim[0];
    ExchangeBalance exchangeBalance(problemeHebdo);

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays - 1; pays++)
    {
        exchangeBalance.add(pays);
    }
}
