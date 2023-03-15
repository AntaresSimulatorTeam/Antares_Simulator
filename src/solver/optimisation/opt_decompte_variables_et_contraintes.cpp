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

#include <antares/logs.h>
#include <antares/emergency.h>

using namespace Antares;

int OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    int mxPaliers = 0;
    ProblemeAResoudre->NombreDeVariables = problemeHebdo->NombreDInterconnexions;

    ProblemeAResoudre->NombreDeVariables += 2 * problemeHebdo->NombreDInterconnexions;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        ProblemeAResoudre->NombreDeVariables
          += problemeHebdo->PaliersThermiquesDuPays[pays]->NombreDePaliersThermiques;

        mxPaliers += problemeHebdo->PaliersThermiquesDuPays[pays]->NombreDePaliersThermiques;

        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->SuiviNiveauHoraire)
        {
            ProblemeAResoudre->NombreDeVariables++;
            ProblemeAResoudre->NombreDeVariables++;
        }

        ProblemeAResoudre->NombreDeVariables += 2;
    }
    ProblemeAResoudre->NombreDeVariables *= nombreDePasDeTempsPourUneOptimisation;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->AccurateWaterValue)
        {
            ProblemeAResoudre->NombreDeVariables += 1;   /* Final Stock Level */
            ProblemeAResoudre->NombreDeVariables += 100; /* Reservoir layers  */
        }
    }

    ProblemeAResoudre->NombreDeContraintes = problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDInterconnexions;

    for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante]->TypeDeContrainteCouplante
            == CONTRAINTE_HORAIRE)
        {
            ProblemeAResoudre->NombreDeContraintes++;
        }
    }
    ProblemeAResoudre->NombreDeContraintes *= nombreDePasDeTempsPourUneOptimisation;

    int nombreDeJoursDansUnIntervalleOptimise;
    if (nombreDePasDeTempsPourUneOptimisation > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        nombreDeJoursDansUnIntervalleOptimise
          = problemeHebdo->NombreDePasDeTemps / problemeHebdo->NombreDePasDeTempsDUneJournee;
    }
    else
        nombreDeJoursDansUnIntervalleOptimise = 1;

    for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante]->TypeDeContrainteCouplante
            == CONTRAINTE_JOURNALIERE)
        {
            ProblemeAResoudre->NombreDeContraintes += nombreDeJoursDansUnIntervalleOptimise;
        }
    }

    if (nombreDePasDeTempsPourUneOptimisation > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante]
                  ->TypeDeContrainteCouplante
                == CONTRAINTE_HEBDOMADAIRE)
            {
                ProblemeAResoudre->NombreDeContraintes++;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        char Pump = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable;
        char TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->TurbinageEntreBornes;
        char MonitorHourlyLev
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->SuiviNiveauHoraire;

        if (!Pump && !TurbEntreBornes && !MonitorHourlyLev
            && problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable)
        {
            ProblemeAResoudre->NombreDeContraintes++;
        }

        if (Pump && !TurbEntreBornes && !MonitorHourlyLev)
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

        if (!Pump && TurbEntreBornes && !MonitorHourlyLev)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;
        }

        if (Pump && TurbEntreBornes && !MonitorHourlyLev)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;
        }

        if (!Pump && TurbEntreBornes && MonitorHourlyLev)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;
        }

        if (Pump && TurbEntreBornes && MonitorHourlyLev)
        {
            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes++;

            ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;
        }
        if (Pump && !TurbEntreBornes && MonitorHourlyLev)
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
              += nombreDePasDeTempsPourUneOptimisation; /* T constraints expressing the level hourly
                                                           variations (14a in the reference
                                                           document) */
        }
        if (!Pump && !TurbEntreBornes && MonitorHourlyLev)
        {
            logs.fatal() << "Level explicit modeling requires flexible generation";
            AntaresSolverEmergencyShutdown();
        }
    }

    for (int areaIndex = 0; areaIndex < problemeHebdo->NombreDePays; areaIndex++)
    {
        const uint nbSTS = (*problemeHebdo->ShortTermStorage)[areaIndex].size();
        ProblemeAResoudre->NombreDeVariables += 3 * nbSTS * nombreDePasDeTempsPourUneOptimisation;
        ProblemeAResoudre->NombreDeContraintes += nbSTS * nombreDePasDeTempsPourUneOptimisation;
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable)
        {
            if (problemeHebdo->TypeDeLissageHydraulique
                == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
            {
                ProblemeAResoudre->NombreDeVariables += nombreDePasDeTempsPourUneOptimisation * 2;
                ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation;
            }

            else if (problemeHebdo->TypeDeLissageHydraulique
                     == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
            {
                ProblemeAResoudre->NombreDeVariables += 2;
                ProblemeAResoudre->NombreDeContraintes += nombreDePasDeTempsPourUneOptimisation * 2;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays]->AccurateWaterValue)
        {
            ProblemeAResoudre->NombreDeContraintes
              += 2; /* Final Stock Level : (1 equivalence cnt + 1 expression cnt )*/
        }
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage(problemeHebdo);
    }

    return mxPaliers;
}
