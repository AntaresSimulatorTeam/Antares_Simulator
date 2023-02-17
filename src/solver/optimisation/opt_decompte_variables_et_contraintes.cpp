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
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"

#include <antares/logs.h>
#include <antares/emergency.h>

using namespace Antares;

void OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo,
                                                                  int* MxPalierThermique)
{
    int Pays;
    int MxPaliers;
    int NombreDePasDeTempsPourUneOptimisation;
    int CntCouplante;
    int NombreDeJoursDansUnIntervalleOptimise;
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre;

    ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    NombreDePasDeTempsPourUneOptimisation = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    MxPaliers = 0;
    ProblemeAResoudre->NombreDeVariables = problemeHebdo->NombreDInterconnexions;

    ProblemeAResoudre->NombreDeVariables += 2 * problemeHebdo->NombreDInterconnexions;

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        ProblemeAResoudre->NombreDeVariables
          += problemeHebdo->PaliersThermiquesDuPays[Pays]->NombreDePaliersThermiques;

        MxPaliers += problemeHebdo->PaliersThermiquesDuPays[Pays]->NombreDePaliersThermiques;

        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
            == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable
            == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeVariables++;
            ProblemeAResoudre->NombreDeVariables++;
        }

        ProblemeAResoudre->NombreDeVariables += 2;
    }
    ProblemeAResoudre->NombreDeVariables *= NombreDePasDeTempsPourUneOptimisation;

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeVariables += 1;   /* Final Stock Level */
            ProblemeAResoudre->NombreDeVariables += 100; /* Reservoir layers  */
        }
    }

    ProblemeAResoudre->NombreDeContraintes = problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDInterconnexions;

    for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante]->TypeDeContrainteCouplante
            == CONTRAINTE_HORAIRE)
        {
            ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    ProblemeAResoudre->NombreDeContraintes *= NombreDePasDeTempsPourUneOptimisation;

    if (NombreDePasDeTempsPourUneOptimisation > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        NombreDeJoursDansUnIntervalleOptimise
          = problemeHebdo->NombreDePasDeTemps / problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
    else
        NombreDeJoursDansUnIntervalleOptimise = 1;

    for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante]->TypeDeContrainteCouplante
            == CONTRAINTE_JOURNALIERE)
        {
            ProblemeAResoudre->NombreDeContraintes += NombreDeJoursDansUnIntervalleOptimise;
        }
    }

    if (NombreDePasDeTempsPourUneOptimisation > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        for (CntCouplante = 0; CntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            if (problemeHebdo->MatriceDesContraintesCouplantes[CntCouplante]
                  ->TypeDeContrainteCouplante
                == CONTRAINTE_HEBDOMADAIRE)
            {
                ProblemeAResoudre->NombreDeContraintes++;
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        char Pump = problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable;
        char TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
        char MonitorHourlyLev
          = problemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire;

        if (Pump == NON_ANTARES && TurbEntreBornes == NON_ANTARES
            && MonitorHourlyLev == NON_ANTARES)
        {
            if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
                == OUI_ANTARES)
            {
                ProblemeAResoudre->NombreDeContraintes++;
            }
        }

        if (Pump == OUI_ANTARES && TurbEntreBornes == NON_ANTARES
            && MonitorHourlyLev == NON_ANTARES)
        {
            ProblemeAResoudre->NombreDeContraintes
              += 2; /* 2 constraints bounding the overall energy generated over the period (10a in
                       the reference document) */
            ProblemeAResoudre
              ->NombreDeContraintes++; /* 1 constraint setting the level variation over the period
                                          (10b in the reference document) */
            ProblemeAResoudre
              ->NombreDeContraintes++; /* 1 constraint bounding the overall energy pumped over the
                                          period (10c in the reference document) */
        }

        if (Pump == NON_ANTARES && TurbEntreBornes == OUI_ANTARES
            && MonitorHourlyLev == NON_ANTARES)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;
        }

        if (Pump == OUI_ANTARES && TurbEntreBornes == OUI_ANTARES
            && MonitorHourlyLev == NON_ANTARES)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;
        }

        if (Pump == NON_ANTARES && TurbEntreBornes == OUI_ANTARES
            && MonitorHourlyLev == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation;
        }

        if (Pump == OUI_ANTARES && TurbEntreBornes == OUI_ANTARES
            && MonitorHourlyLev == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation;
        }
        if (Pump == OUI_ANTARES && TurbEntreBornes == NON_ANTARES
            && MonitorHourlyLev == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeContraintes
              += 2; /* 2 constraints bounding the overall energy generated over the period (10a in
                       the reference document) */
            ProblemeAResoudre
              ->NombreDeContraintes++; /* 1 constraint setting the level variation over the period
                                          (10b in the reference document) */
            ProblemeAResoudre
              ->NombreDeContraintes++; /* 1 constraint bounding the overall energy pumped over the
                                          period (10c in the reference document) */
            ProblemeAResoudre->NombreDeContraintes
              += NombreDePasDeTempsPourUneOptimisation; /* T constraints expressing the level hourly
                                                           variations (14a in the reference
                                                           document) */
        }
        if (Pump == NON_ANTARES && TurbEntreBornes == NON_ANTARES
            && MonitorHourlyLev == OUI_ANTARES)
        {
            logs.fatal() << "Level explicit modeling requires flexible generation";
            AntaresSolverEmergencyShutdown();
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable
            == OUI_ANTARES)
        {
            if (problemeHebdo->TypeDeLissageHydraulique
                == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
            {
                ProblemeAResoudre->NombreDeVariables += NombreDePasDeTempsPourUneOptimisation * 2;
                ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation;
            }

            else if (problemeHebdo->TypeDeLissageHydraulique
                     == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
            {
                ProblemeAResoudre->NombreDeVariables += 2;
                ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation * 2;
            }
        }
    }

    for (Pays = 0; Pays < problemeHebdo->NombreDePays; Pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[Pays]->AccurateWaterValue == OUI_ANTARES)
        {
            ProblemeAResoudre->NombreDeContraintes
              += 2; /* Final Stock Level : (1 equivalence cnt + 1 expression cnt )*/
        }
    }

    *MxPalierThermique = MxPaliers;

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES)
    {
        OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage(problemeHebdo);
    }

    return;
}
