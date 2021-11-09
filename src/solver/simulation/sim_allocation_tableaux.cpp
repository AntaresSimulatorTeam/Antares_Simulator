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
#include <antares/study/study.h>
#include "simulation.h"
#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_extern_variables_globales.h"

using namespace Antares;

static void AllocateResultsForEconomicMode(void)
{
    auto& study = *Data::Study::Current::Get();
    const size_t sizeOfLongHours = study.runtime->nbHoursPerYear * sizeof(int);
    const size_t sizeOfDoubleHours = study.runtime->nbHoursPerYear * sizeof(double);
    RESULTATS_PAR_INTERCONNEXION* rpNtc;

    uint i;

    ResultatsParInterconnexion = (RESULTATS_PAR_INTERCONNEXION**)MemAlloc(
      (1 + study.runtime->interconnectionsCount) * sizeof(void*));
    for (i = 0; i != study.runtime->interconnectionsCount; i++)
    {
        rpNtc = (RESULTATS_PAR_INTERCONNEXION*)MemAlloc(sizeof(RESULTATS_PAR_INTERCONNEXION));
        ResultatsParInterconnexion[i] = rpNtc;

        rpNtc->TransitMoyen = (double*)MemAlloc(sizeOfDoubleHours);
        rpNtc->TransitMinimum = (double*)MemAlloc(sizeOfDoubleHours);
        rpNtc->TransitMinimumNo = (int*)MemAlloc(sizeOfLongHours);
        rpNtc->TransitMaximum = (double*)MemAlloc(sizeOfDoubleHours);
        rpNtc->TransitMaximumNo = (int*)MemAlloc(sizeOfLongHours);
        rpNtc->TransitStdDev = (double*)MemAlloc(sizeOfDoubleHours);
        rpNtc->TransitAnnuel = (double*)MemAlloc(study.runtime->nbYears * sizeof(double));
        rpNtc->TransitMoyenRecalculQuadratique = (double*)MemAlloc(sizeOfDoubleHours);
        rpNtc->VariablesDualesMoyennes = (double*)MemAlloc(sizeOfDoubleHours);
        rpNtc->RenteHoraire = (double*)MemAlloc(sizeOfDoubleHours);
    }

    ResultatsParContrainteCouplante = (RESULTATS_PAR_CONTRAINTE_COUPLANTE**)MemAlloc(
      study.runtime->bindingConstraintCount * sizeof(void*));
    for (i = 0; i != study.runtime->bindingConstraintCount; ++i)
    {
        ResultatsParContrainteCouplante[i] = (RESULTATS_PAR_CONTRAINTE_COUPLANTE*)MemAlloc(
          sizeof(RESULTATS_PAR_CONTRAINTE_COUPLANTE));
        ResultatsParContrainteCouplante[i]->VariablesDualesMoyennes
          = (double*)MemAlloc(sizeOfDoubleHours);
    }
}

static void DeallocateResultsForEconomicMode(void)
{
    auto& study = *Data::Study::Current::Get();
    RESULTATS_PAR_INTERCONNEXION* rpNtc;
    uint i;

    for (i = 0; i != study.runtime->interconnectionsCount; i++)
    {
        rpNtc = ResultatsParInterconnexion[i];

        MemFree(rpNtc->TransitMoyen);
        MemFree(rpNtc->TransitMinimum);
        MemFree(rpNtc->TransitMinimumNo);
        MemFree(rpNtc->TransitMaximum);
        MemFree(rpNtc->TransitMaximumNo);
        MemFree(rpNtc->TransitStdDev);
        MemFree(rpNtc->TransitAnnuel);
        MemFree(rpNtc->TransitMoyenRecalculQuadratique);
        MemFree(rpNtc->VariablesDualesMoyennes);
        MemFree(rpNtc->RenteHoraire);

        MemFree(rpNtc);
    }
    MemFree(ResultatsParInterconnexion);
    ResultatsParInterconnexion = NULL;

    for (i = 0; i != study.runtime->bindingConstraintCount; ++i)
    {
        MemFree(ResultatsParContrainteCouplante[i]->VariablesDualesMoyennes);
        MemFree(ResultatsParContrainteCouplante[i]);
    }
    MemFree(ResultatsParContrainteCouplante);
    ResultatsParContrainteCouplante = NULL;
}

void SIM_AllocationTableaux()
{
    uint i;
    auto& study = *Data::Study::Current::Get();

    DonneesParPays = (DONNEES_PAR_PAYS**)MemAlloc(study.areas.size() * sizeof(DONNEES_PAR_PAYS*));
    for (i = 0; i < study.areas.size(); ++i)
        DonneesParPays[i] = (DONNEES_PAR_PAYS*)MemAlloc(sizeof(DONNEES_PAR_PAYS));

    ValeursGenereesParPays
      = (VALEURS_GENEREES_PAR_PAYS***)MemAlloc(study.maxNbYearsInParallel * sizeof(void*));
    NumeroChroniquesTireesParPays
      = (NUMERO_CHRONIQUES_TIREES_PAR_PAYS***)MemAlloc(study.maxNbYearsInParallel * sizeof(void*));

    for (uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
    {
        ValeursGenereesParPays[numSpace]
          = (VALEURS_GENEREES_PAR_PAYS**)MemAlloc(study.areas.size() * sizeof(void*));
        NumeroChroniquesTireesParPays[numSpace]
          = (NUMERO_CHRONIQUES_TIREES_PAR_PAYS**)MemAlloc(study.areas.size() * sizeof(void*));
        for (i = 0; i < study.areas.size(); ++i)
        {
            auto& area = *study.areas.byIndex[i];

            NumeroChroniquesTireesParPays[numSpace][i]
              = (NUMERO_CHRONIQUES_TIREES_PAR_PAYS*)MemAlloc(
                sizeof(NUMERO_CHRONIQUES_TIREES_PAR_PAYS));
            ValeursGenereesParPays[numSpace][i]
              = (VALEURS_GENEREES_PAR_PAYS*)MemAlloc(sizeof(VALEURS_GENEREES_PAR_PAYS));

            NumeroChroniquesTireesParPays[numSpace][i]->ThermiqueParPalier
              = (int*)MemAlloc(area.thermal.clusterCount() * sizeof(int));
            NumeroChroniquesTireesParPays[numSpace][i]->RenouvelableParPalier
              = (int*)MemAlloc(area.renewable.clusterCount() * sizeof(int));
            ValeursGenereesParPays[numSpace][i]->HydrauliqueModulableQuotidien
              = (double*)MemAlloc(study.runtime->nbDaysPerYear * sizeof(double));
            ValeursGenereesParPays[numSpace][i]->AleaCoutDeProductionParPalier
              = (double*)MemAlloc(area.thermal.clusterCount() * sizeof(double));
            if (area.hydro.reservoirManagement)
            {
                ValeursGenereesParPays[numSpace][i]->NiveauxReservoirsDebutJours
                  = (double*)MemAlloc(study.runtime->nbDaysPerYear * sizeof(double));
                ValeursGenereesParPays[numSpace][i]->NiveauxReservoirsFinJours
                  = (double*)MemAlloc(study.runtime->nbDaysPerYear * sizeof(double));
            }
        }
    }
    NumeroChroniquesTireesParInterconnexion
      = (NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION**)MemAlloc(study.maxNbYearsInParallel
                                                                * sizeof(void*));
    const auto intercoCount = study.runtime->interconnectionsCount;
    for (uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
    {
        NumeroChroniquesTireesParInterconnexion[numSpace]
          = (NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION*)MemAlloc(
            intercoCount * sizeof(NUMERO_CHRONIQUES_TIREES_PAR_INTERCONNEXION*));
    }

    if (not study.parameters.adequacyDraft())
    {
        AllocateResultsForEconomicMode();
    }
}

void SIM_DesallocationTableaux()
{
    auto studyptr = Data::Study::Current::Get();
    if (!(!studyptr))
    {
        auto& study = *studyptr;
        for (uint i = 0; i < study.areas.size(); ++i)
            MemFree(DonneesParPays[i]);

        for (uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
        {
            for (uint i = 0; i < study.areas.size(); ++i)
            {
                auto& area = *study.areas.byIndex[i];

                MemFree(NumeroChroniquesTireesParPays[numSpace][i]->ThermiqueParPalier);
                MemFree(NumeroChroniquesTireesParPays[numSpace][i]->RenouvelableParPalier);
                MemFree(NumeroChroniquesTireesParPays[numSpace][i]);
                MemFree(ValeursGenereesParPays[numSpace][i]->HydrauliqueModulableQuotidien);

                if (area.hydro.reservoirManagement)
                {
                    MemFree(ValeursGenereesParPays[numSpace][i]->NiveauxReservoirsDebutJours);
                    MemFree(ValeursGenereesParPays[numSpace][i]->NiveauxReservoirsFinJours);
                }

                MemFree(ValeursGenereesParPays[numSpace][i]);
            }
            MemFree(ValeursGenereesParPays[numSpace]);
            MemFree(NumeroChroniquesTireesParPays[numSpace]);
        }
        for (uint numSpace = 0; numSpace < study.maxNbYearsInParallel; numSpace++)
        {
            MemFree(NumeroChroniquesTireesParInterconnexion[numSpace]);
        }
    }
    MemFree(NumeroChroniquesTireesParPays);
    MemFree(ValeursGenereesParPays);
    MemFree(NumeroChroniquesTireesParInterconnexion);
    NumeroChroniquesTireesParPays = NULL;
    ValeursGenereesParPays = NULL;
    NumeroChroniquesTireesParInterconnexion = nullptr;

    MemFree(DonneesParPays);
    DonneesParPays = NULL;

    if (not studyptr->parameters.adequacyDraft())
    {
        DeallocateResultsForEconomicMode();
    }
}
