/*
** Copyright 2007-2018 RTE
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
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include <math.h>
#include <yuni/core/math.h>
#include <limits.h>

#include "spx_constantes_externes.h"

#define EPSILON_DEFAILLANCE 1e-3

using namespace Yuni;

void OPT_InitialiserLesBornesDesVariablesDuProblemeLineaireCoutsDeDemarrage(
  PROBLEME_HEBDO* problemeHebdo,
  const int PremierPdtDeLIntervalle,
  const int DernierPdtDeLIntervalle)
{
    int PdtHebdo;
    int PdtJour;
    int Pays;
    int Palier;
    int Var;
    int Index;
    double* AdresseDuResultat;
    double** AdresseOuPlacerLaValeurDesVariablesOptimisees;
    double* Xmin;
    double* Xmax;
    int t1;
    int NombreDePasDeTempsPourUneOptimisation;
    int t1moins1;
    int* NombreMaxDeGroupesEnMarcheDuPalierThermique;
    int* NombreMinDeGroupesEnMarcheDuPalierThermique;

    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    PDISP_ET_COUTS_HORAIRES_PAR_PALIER* PuissanceDisponibleEtCout;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    AdresseOuPlacerLaValeurDesVariablesOptimisees
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees;
    Xmin = ProblemeAResoudre->Xmin;
    Xmax = ProblemeAResoudre->Xmax;

    for (PdtHebdo = PremierPdtDeLIntervalle, PdtJour = 0; PdtHebdo < DernierPdtDeLIntervalle;
         PdtHebdo++, PdtJour++)
    {
        CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];

        for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
        {
            PaliersThermiquesDuPays = problemeHebdo->PaliersThermiquesDuPays[Pays];

            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index];
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                NombreMaxDeGroupesEnMarcheDuPalierThermique
                  = PuissanceDisponibleEtCout->NombreMaxDeGroupesEnMarcheDuPalierThermique;
                NombreMinDeGroupesEnMarcheDuPalierThermique
                  = PuissanceDisponibleEtCout->NombreMinDeGroupesEnMarcheDuPalierThermique;

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique[Palier];
                Xmax[Var] = NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo];
                Xmin[Var] = NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo];

                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ProductionThermique[PdtHebdo]
                                        ->NombreDeGroupesEnMarcheDuPalier[Index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique[Palier];
                Xmax[Var] = LINFINI_ANTARES;
                Xmin[Var] = 0;
                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ProductionThermique[PdtHebdo]
                                        ->NombreDeGroupesQuiDemarrentDuPalier[Index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique[Palier];
                Xmax[Var] = LINFINI_ANTARES;
                Xmin[Var] = 0;
                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ProductionThermique[PdtHebdo]
                                        ->NombreDeGroupesQuiSArretentDuPalier[Index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;

                Var
                  = CorrespondanceVarNativesVarOptim
                      ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique[Palier];
                Xmin[Var] = 0;
#if VARIABLES_MMOINS_MOINS_BORNEES_DES_2_COTES != OUI_ANTARES
                Xmax[Var] = LINFINI_ANTARES;
#else
                t1 = PdtHebdo;
                t1moins1 = t1 - 1;
                if (t1moins1 < 0)
                    t1moins1 = NombreDePasDeTempsPourUneOptimisation + t1moins1;
                Xmax[Var] = 0;
                if (NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                      - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1]
                    > 0)
                {
                    Xmax[Var] = NombreMaxDeGroupesEnMarcheDuPalierThermique[t1moins1]
                                - NombreMaxDeGroupesEnMarcheDuPalierThermique[t1];
                }
#endif
                AdresseDuResultat = &(problemeHebdo->ResultatsHoraires[Pays]
                                        ->ProductionThermique[PdtHebdo]
                                        ->NombreDeGroupesQuiTombentEnPanneDuPalier[Index]);
                AdresseOuPlacerLaValeurDesVariablesOptimisees[Var] = AdresseDuResultat;
            }
        }
    }
}
