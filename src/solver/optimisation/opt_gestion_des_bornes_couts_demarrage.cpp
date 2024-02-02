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
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    std::vector<double*>& AdresseOuPlacerLaValeurDesVariablesOptimisees
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;
    std::vector<double>& Xmin = ProblemeAResoudre->Xmin;
    std::vector<double>& Xmax = ProblemeAResoudre->Xmax;

    for (int pdtHebdo = PremierPdtDeLIntervalle, pdtJour = 0; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++, pdtJour++)
    {
        const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          =  problemeHebdo->CorrespondanceVarNativesVarOptim[pdtJour];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
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
                            .NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[palier];
                Xmax[var] = NombreMaxDeGroupesEnMarcheDuPalierThermique[pdtHebdo];
                Xmin[var] = NombreMinDeGroupesEnMarcheDuPalierThermique[pdtHebdo];

                double* adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                                .ProductionThermique[pdtHebdo]
                                                .NombreDeGroupesEnMarcheDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

                var = CorrespondanceVarNativesVarOptim
                        .NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[palier];
                Xmax[var] = LINFINI_ANTARES;
                Xmin[var] = 0;
                adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                        .ProductionThermique[pdtHebdo]
                                        .NombreDeGroupesQuiDemarrentDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

                var = CorrespondanceVarNativesVarOptim
                        .NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[palier];
                Xmax[var] = LINFINI_ANTARES;
                Xmin[var] = 0;
                adresseDuResultat = &(problemeHebdo->ResultatsHoraires[pays]
                                        .ProductionThermique[pdtHebdo]
                                        .NombreDeGroupesQuiSArretentDuPalier[index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = adresseDuResultat;

                var
                  = CorrespondanceVarNativesVarOptim
                      .NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[palier];
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
