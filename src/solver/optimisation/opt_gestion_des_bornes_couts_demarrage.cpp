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
#include "../simulation/sim_structure_probleme_economique.h"
#include "opt_fonctions.h"

using namespace Yuni;

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  const int PremierPdtDeLIntervalle,
  const int DernierPdtDeLIntervalle)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;
    std::vector<double>& Xmin = ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = ProblemeAResoudre->Xmax;

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];

            for (int index = 0; index < PaliersThermiquesDuPays.NombreDePaliersThermiques; index++)
            {
                const PDISP_ET_COUTS_HORAIRES_PAR_PALIER& PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays.PuissanceDisponibleEtCout[index];
                const int palier
                  = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
                const std::vector<int>& NombreMaxDeGroupesEnMarcheDuPalierThermique
                  = PuissanceDisponibleEtCout.NombreMaxDeGroupesEnMarcheDuPalierThermique;
                const std::vector<int>& NombreMinDeGroupesEnMarcheDuPalierThermique
                  = PuissanceDisponibleEtCout.NombreMinDeGroupesEnMarcheDuPalierThermique;

                int var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                Xmax[var] = NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
                Xmin[var] = NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo];

                double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                                .ProductionThermique[pdtHebdo]
                                                .NombreDeGroupesEnMarcheDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[palier];
                Xmax[var] = LINFINI_ANTARES;
                Xmin[var] = 0;
                adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                        .ProductionThermique[pdtHebdo]
                                        .NombreDeGroupesQuiDemarrentDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

                var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[palier];
                Xmax[var] = LINFINI_ANTARES;
                Xmin[var] = 0;
                adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                        .ProductionThermique[pdtHebdo]
                                        .NombreDeGroupesQuiSArretentDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

                var
                  = CorrespondanceVarNativesVarOptim
                      ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[palier];
                Xmin[var] = 0;
                int t1 = pdtHebdo;
                int t1moins1 = t1 - 1;
                if (t1moins1 < 0)
                    t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;
                Xmax[var] = 0;
                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                      - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                    > 0)
                {
                    Xmax[var] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                                - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                }
                adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                        .ProductionThermique[pdtHebdo]
                                        .NombreDeGroupesQuiTombentEnPanneDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;
            }
        }
    }
}
