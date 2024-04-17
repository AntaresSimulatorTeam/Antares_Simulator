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

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"

#include "antares/solver/optimisation/opt_fonctions.h"

#include <antares/logs/logs.h>
#include <antares/antares/fatal-error.h>

using namespace Antares;

int OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser(PROBLEME_HEBDO* problemeHebdo)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    int mxPaliers = 0;
    ProblemeAResoudre->NombreDeVariables = problemeHebdo->NombreDInterconnexions;

    ProblemeAResoudre->NombreDeVariables += 2 * problemeHebdo->NombreDInterconnexions;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        ProblemeAResoudre->NombreDeVariables
          += problemeHebdo->PaliersThermiquesDuPays[pays].NombreDePaliersThermiques;

        mxPaliers += problemeHebdo->PaliersThermiquesDuPays[pays].NombreDePaliersThermiques;

        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        {
            ProblemeAResoudre->NombreDeVariables++;
        }

        if (problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
        {
            ProblemeAResoudre->NombreDeVariables++;
            ProblemeAResoudre->NombreDeVariables++;
        }

        ProblemeAResoudre->NombreDeVariables += 2;
    }
    ProblemeAResoudre->NombreDeVariables *= nombreDePasDeTempsPourUneOptimisation;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            ProblemeAResoudre->NombreDeVariables += 1;   /* Final Stock Level */
            ProblemeAResoudre->NombreDeVariables += 100; /* Reservoir layers  */
        }
    }

    ProblemeAResoudre->NombreDeContraintes = problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDePays;

    ProblemeAResoudre->NombreDeContraintes += problemeHebdo->NombreDInterconnexions;

    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante].TypeDeContrainteCouplante
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

    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante].TypeDeContrainteCouplante
            == CONTRAINTE_JOURNALIERE)
        {
            ProblemeAResoudre->NombreDeContraintes += nombreDeJoursDansUnIntervalleOptimise;
        }
    }

    if (nombreDePasDeTempsPourUneOptimisation > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            if (problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante]
                  .TypeDeContrainteCouplante
                == CONTRAINTE_HEBDOMADAIRE)
            {
                ProblemeAResoudre->NombreDeContraintes++;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        char Pump = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable;
        char TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;
        char MonitorHourlyLev
          = problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire;

        if (!Pump && !TurbEntreBornes && !MonitorHourlyLev
            && problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
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
            const std::string areaName(problemeHebdo->NomsDesPays[pays]);
            throw FatalError("Level explicit modeling requires flexible generation in area " + areaName);
        }
    }

    // Short term storage
    {
        const uint nbSTS = problemeHebdo->NumberOfShortTermStorages;
        // Level, injection, withdrawal
        ProblemeAResoudre->NombreDeVariables += 3 * nbSTS * nombreDePasDeTempsPourUneOptimisation;
        // Level equation (Level[h+1] = Level[h] + ...)
        ProblemeAResoudre->NombreDeContraintes += nbSTS * nombreDePasDeTempsPourUneOptimisation;
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
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

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            ProblemeAResoudre->NombreDeContraintes
              += 2; /* Final Stock Level : (1 equivalence cnt + 1 expression cnt )*/
        }
    }

    //Reserves
    //OPT_DecompteDesVariablesEtDesContraintesReserves(problemeHebdo);
    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage(problemeHebdo);
        OPT_DecompteDesVariablesEtDesContraintesReserves(problemeHebdo);
    }

    return mxPaliers;
}
