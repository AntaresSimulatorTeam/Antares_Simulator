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

namespace Antares
{

using namespace Data;

struct setOfParallelYears
{
    // Un lot d'année à exécuter en parallèle.
    // En fonction d'une éventuelle play-list, certaines seront jouées et d'autres non.

public:
    std::vector<unsigned int> setsSizes;
    // Numeros des annees en parallele pour ce lot (certaines ne seront pas jouées en cas de
    // play-list "trouée")
    std::vector<unsigned int> yearsIndices;

    // Une annee doit-elle être rejouée ?
    std::map<uint, bool> yearFailed;

    // Associe le numero d'une année jouée à l'indice de l'espace
    std::map<unsigned int, unsigned int> performedYearToSpace;

    // L'inverse : pour une année jouée, associe l'indice de l'espace au numero de l'année
    std::map<unsigned int, unsigned int> spaceToPerformedYear;

    // Pour chaque année, est-elle la première à devoir être jouée dans son lot d'années ?
    std::map<unsigned int, bool> isFirstPerformedYearOfASet;

    // Pour chaque année du lot, est-elle jouée ou non ?
    std::map<unsigned int, bool> isYearPerformed;

    // Nbre d'années en parallele vraiment jouées pour ce lot
    unsigned int nbPerformedYears;

    // Nbre d'années en parallele jouées ou non pour ce lot
    unsigned int nbYears;

    // Regenere-t-on des times series avant de jouer les annees du lot courant
    bool regenerateTS;

    // Annee a passer a la fonction "regenerateTimeSeries<false>(y)" (si regenerateTS is "true")
    unsigned int yearForTSgeneration;
};

/*!
** \brief Temporary Area List Holder
**
** \warning TODO: REMOVE THIS CLASS ONCE THE TSGEN HAS BEEN DECOUPLED FROM SOLVER
**
** To compute the sets of parallel years while using the Time Series Generator
** we need to know in advance if the area's thermal clusters will be refresehd during generation
** Although, since the max number of parallel years is needed to create the areas, this info
** is not available yet (and tht's why, originally, this calculation was done twice)
**
** This temporary holder will load the area list and will check for each one if
** their clusters will require refreshing during generation
*/
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