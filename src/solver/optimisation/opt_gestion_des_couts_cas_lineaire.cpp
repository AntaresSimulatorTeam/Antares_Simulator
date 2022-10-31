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

#include <yuni/yuni.h>
#include <yuni/core/math.h>
#include <limits>
#include <antares/study/area/scratchpad.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include "../aleatoire/alea_fonctions.h"

#include "spx_constantes_externes.h"

static void ComputeMinMaxValueForLoad(PROBLEME_HEBDO* ProblemeHebdo,
                                      const int PremierPasDeTempsHebdo,
                                      const int DernierPasDeTempsHebdo,
                                      uint numSpace)
{
    using namespace Antares::Data;

    double d;

    auto& study = *Antares::Data::Study::Current::Get();
    const Area::Map::const_iterator end = study.areas.end();
    for (Area::Map::const_iterator i = study.areas.begin(); i != end; ++i)
    {
        const Area& area = *(i->second);
        auto& scratchpad = *(area.scratchpad[numSpace]);
        scratchpad.consoMin = +std::numeric_limits<double>::infinity();
        scratchpad.consoMax = -std::numeric_limits<double>::infinity();

        for (int i = PremierPasDeTempsHebdo; i < DernierPasDeTempsHebdo; ++i)
        {
            d = ProblemeHebdo->ConsommationsAbattues[i]->ConsommationAbattueDuPays[area.index];
            if (d < scratchpad.consoMin)
                scratchpad.consoMin = d;
            if (d > scratchpad.consoMax)
                scratchpad.consoMax = d;
        }
    }
}

void OPT_InitialiserLesCoutsLineaire(PROBLEME_HEBDO* ProblemeHebdo,
                                     const int PremierPdtDeLIntervalle,
                                     const int DernierPdtDeLIntervalle,
                                     uint numSpace)
{
    int PdtJour;
    int Interco;
    int Pays;
    int Palier;
    int Var;
    int Index;
    int PdtHebdo;
    double P;
    int layerindex;
    CORRESPONDANCES_DES_VARIABLES* CorrespondanceVarNativesVarOptim;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    COUTS_DE_TRANSPORT* CoutDeTransport;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;
    auto& study = *Antares::Data::Study::Current::Get();

    ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

    PdtJour = 0;

    memset((char*)ProblemeAResoudre->CoutQuadratique,
           0,
           ProblemeAResoudre->NombreDeVariables * sizeof(double));

    ComputeMinMaxValueForLoad(
      ProblemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle, numSpace);

    for (PdtHebdo = PremierPdtDeLIntervalle; PdtHebdo < DernierPdtDeLIntervalle; PdtHebdo++)
    {
        CorrespondanceVarNativesVarOptim = ProblemeHebdo->CorrespondanceVarNativesVarOptim[PdtJour];

        for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
        {
            CoutDeTransport = ProblemeHebdo->CoutDeTransport[Interco];

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDeLInterconnexion[Interco];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var] = 0.0;
            }

            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion[Interco];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = CoutDeTransport->CoutDeTransportOrigineVersExtremite[PdtHebdo];
                }
                Var = CorrespondanceVarNativesVarOptim
                        ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion[Interco];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = CoutDeTransport->CoutDeTransportExtremiteVersOrigine[PdtHebdo];
                }
            }
        }

        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; ++Pays)
        {
            assert((unsigned int)Pays < study.areas.size());

            PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
            for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++)
            {
                Palier
                  = PaliersThermiquesDuPays->NumeroDuPalierDansLEnsembleDesPaliersThermiques[Index];
                Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDuPalierThermique[Palier];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]
                          ->CoutHoraireDeProductionDuPalierThermique[PdtHebdo];
                }
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeLaProdHyd[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                ProblemeAResoudre->CoutLineaire[Var] = 0.0;

            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
#ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
                if (study.runtime->hydroCostByAreaShouldBeInfinite[Pays])
                    ProblemeAResoudre->CoutLineaire[Var] = study.runtime->globalMaximumCost;
#endif

                ProblemeAResoudre->CoutLineaire[Var]
                  = ProblemeHebdo
                      ->BruitSurCoutHydraulique[Pays][ProblemeHebdo->HeureDansLAnnee + PdtHebdo];

                if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue
                    == NON_ANTARES)
                    ProblemeAResoudre->CoutLineaire[Var]
                      += ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                           ->WeeklyWaterValueStateRegular;
            }

            if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                == OUI_ANTARES)
            {
                if (ProblemeHebdo->TypeDeLissageHydraulique
                    == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
                {
                    P = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                          ->PenalisationDeLaVariationDeProductionHydrauliqueSurSommeDesVariations;

                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                    if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                        ProblemeAResoudre->CoutLineaire[Var] = P;
                    Var = CorrespondanceVarNativesVarOptim
                            ->NumeroDeVariablesVariationHydALaHausse[Pays];
                    if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                        ProblemeAResoudre->CoutLineaire[Var] = P;
                }
                else if (ProblemeHebdo->TypeDeLissageHydraulique
                         == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
                {
                    if (PdtJour == 0)
                    {
                        P = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                              ->PenalisationDeLaVariationDeProductionHydrauliqueSurVariationMax;
                        Var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariablesVariationHydALaBaisse[Pays];
                        if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                            ProblemeAResoudre->CoutLineaire[Var] = P;
                        Var = CorrespondanceVarNativesVarOptim
                                ->NumeroDeVariablesVariationHydALaHausse[Pays];
                        if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                            ProblemeAResoudre->CoutLineaire[Var] = -P;
                    }
                }
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDePompage[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                /* Sets the cost of the pumping variable when such a variable is actually defined
                (i.e. Var>=0)


                1-   When the "AccurateWaterValue" optimization mode is not used, the pumping
                variable must be given an explicit meaningful value, because no reservoir level
                variables are defined in this case. The pumping cost is based on the water value
                found for the initial reservoir level (with a sign change and weighted by the
                pumping ratio) In the eventuality of a zero-value water configuration, the pumping
                cost is then uplifted by an "espilon" component whose definition is given in the
                next section.

                2-   When the "AccurateWaterValue" optimization mode is used, the economic
                    contribution of pumping is mainly described through the cost set for the
                reservoir level variable ; as a consequence, the pumping variable is given only a
                residual "epsilon" cost based on the hydro generating "economic noise" The residual
                pumping cost is set to 2*fabs(generating noise), so as to make sure that, if the
                pumping ratio is equal to 1.0, no meaningless situations involving pumping and
                generating equal power amounts in the same location shall ever occur.

                */

                if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue
                    == NON_ANTARES)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                          ->WeeklyWaterValueStateRegular;

                    ProblemeAResoudre->CoutLineaire[Var]
                      *= ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PumpingRatio;
                    ProblemeAResoudre->CoutLineaire[Var] *= -1.;

                    ProblemeAResoudre->CoutLineaire[Var]
                      += 2.
                         * fabs(ProblemeHebdo
                                  ->BruitSurCoutHydraulique[Pays][ProblemeHebdo->HeureDansLAnnee
                                                                  + PdtHebdo]);
                }
                else
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = 2.
                        * fabs(ProblemeHebdo
                                 ->BruitSurCoutHydraulique[Pays][ProblemeHebdo->HeureDansLAnnee
                                                                 + PdtHebdo]);
                }
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeDebordement[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                /* Sets the cost of the overflow variable when such a variable is actually defined
               (i.e. Var>=0)


               1-   When the "AccurateWaterValue" optimization mode is not used, the overflow
               variable must be given a cost translating the fact that overflowing is at the same
               time:
                - A loss of a storage oppportunity, whose value is in this case the water value
               found for the initial level.
                - A spillage of power, whose value is the reference spillage cost.

               2-   When the "AccurateWaterValue" optimization mode is used, the economic loss of
               storage oppportunity incurred when overflowing is modelled through the costs of the
               reservoir level variables. The cost of the overflow variable involves therefore only
               a spillage component.

              */

                if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue
                    == NON_ANTARES)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = ProblemeHebdo->CoutDeDefaillanceNegative[Pays];

                    ProblemeAResoudre->CoutLineaire[Var]
                      += ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                           ->WeeklyWaterValueStateRegular;
                }
                else
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = ProblemeHebdo->CoutDeDefaillanceNegative[Pays];
                }
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariablesDeNiveau[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var] = 0;
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillancePositive[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var]
                  = ProblemeHebdo->CoutDeDefaillancePositive[Pays];
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceNegative[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var]
                  = ProblemeHebdo->CoutDeDefaillanceNegative[Pays];
            }

            Var = CorrespondanceVarNativesVarOptim->NumeroDeVariableDefaillanceEnReserve[Pays];

            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var]
                  = ProblemeHebdo->CoutDeDefaillanceEnReserve[Pays];
            }
        }

        PdtJour++;
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; ++Pays)
    {
        if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        {
            Var = ProblemeHebdo->NumeroDeVariableStockFinal[Pays];
            if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
            {
                ProblemeAResoudre->CoutLineaire[Var] = 0;
            }

            for (layerindex = 0; layerindex < 100; layerindex++)
            {
                Var = ProblemeHebdo->NumeroDeVariableDeTrancheDeStock[Pays][layerindex];
                if (Var >= 0 && Var < ProblemeAResoudre->NombreDeVariables)
                {
                    ProblemeAResoudre->CoutLineaire[Var]
                      = -ProblemeHebdo->CaracteristiquesHydrauliques[Pays]
                           ->WaterLayerValues[layerindex];
                }
            }
        }
    }

    if (ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        OPT_InitialiserLesCoutsLineaireCoutsDeDemarrage(
          ProblemeHebdo, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    }

    return;
}
