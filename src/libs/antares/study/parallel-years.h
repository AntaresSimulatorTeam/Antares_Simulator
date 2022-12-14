/*
** Copyright 2007-2022 RTE
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
#pragma once

#include <vector>
#include "../../../solver/simulation/solver.utils.h" // KVR se debarraser de ce include

namespace Antares
{

using namespace Data;
// using namespace Solver::Simulation;

struct setOfParallelYears
{
    // Un lot d'ann�e � ex�cuter en parall�le.
    // En fonction d'une �ventuelle play-list, certaines seront jou�es et d'autres non.

public:
    std::vector<unsigned int> setsSizes;
    // Numeros des annees en parallele pour ce lot (certaines ne seront pas jou�es en cas de
    // play-list "trou�e")
    std::vector<unsigned int> yearsIndices;

    // Une annee doit-elle �tre rejou�e ?
    std::map<uint, bool> yearFailed;

    // Associe le numero d'une ann�e jou�e � l'indice de l'espace
    std::map<unsigned int, unsigned int> performedYearToSpace;

    // L'inverse : pour une ann�e jou�e, associe l'indice de l'espace au numero de l'ann�e
    std::map<unsigned int, unsigned int> spaceToPerformedYear;

    // Pour chaque ann�e, est-elle la premi�re � devoir �tre jou�e dans son lot d'ann�es ?
    std::map<unsigned int, bool> isFirstPerformedYearOfASet;

    // Pour chaque ann�e du lot, est-elle jou�e ou non ?
    std::map<unsigned int, bool> isYearPerformed;

    // Nbre d'ann�es en parallele vraiment jou�es pour ce lot
    unsigned int nbPerformedYears;

    // Nbre d'ann�es en parallele jou�es ou non pour ce lot
    unsigned int nbYears;

    // Regenere-t-on des times series avant de jouer les annees du lot courant
    bool regenerateTS;

    // Annee a passer a la fonction "regenerateTimeSeries<false>(y)" (si regenerateTS is "true")
    unsigned int yearForTSgeneration;
};

class TempAreaListHolder
{
public:
    TempAreaListHolder() = default;

    bool checkThermalTSGeneration(YString folder_);

private:
    void loadAreaList();
    YString inputFolder;
    std::vector<Yuni::String> areaNames;

};

class SetsOfParallelYearCalculator
{
public:

    friend class SetsOfParallelYearCalculatorWrapper; 
    
    SetsOfParallelYearCalculator(
        bool forceParallel_,
        bool enableParallel_,
        uint forcedNbOfParallelYears_,
        bool thermalTSRefresh_, // Temporary parameter needed. To be removed once TSGen will be detached from simulator
        Parameters& params_)
    : forceParallel{forceParallel_},
    enableParallel {enableParallel_},
    forcedNbOfParallelYears{forcedNbOfParallelYears_},
    thermalTSRefresh{thermalTSRefresh_},
    p{params_}{
        this->build();
    }

        
    bool allSetsParallelYearsHaveSameSize();

    uint getForcedNbOfParallelYears() const
    {
        return forcedNbOfParallelYears; 
    } 

    uint getMinNbParallelYearsForGUI() const
    {
        return computeMinNbParallelYears();
    }

    std::vector<setOfParallelYears> getSetsOfParallelYears() const
    {
        return setsOfParallelYears;
    }

    uint getNbYearsReallyPerformed() const
    {
        return nbYearsReallyPerformed;
    }

private:
    
    void build();

    void computeRawNbParallelYear();
    void limitNbOfParallelYearsbyMinRefreshSpan();
    
    void buildSetsOfParallelYears();

    uint computeMinNbParallelYears() const;
    void computeForcedNbYearsInParallelYearSet();

    bool isRefreshNeededForCurrentYear(uint y);
  
    std::vector<setOfParallelYears> setsOfParallelYears;
   
    bool forceParallel;
    bool enableParallel;
    uint forcedNbOfParallelYears;
    bool thermalTSRefresh;
    Parameters& p;
    uint nbYearsReallyPerformed{0};

};

} // namespace Anatares