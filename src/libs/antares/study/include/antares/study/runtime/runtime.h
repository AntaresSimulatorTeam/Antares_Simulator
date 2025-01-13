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
#ifndef __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__
#define __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__

#include <string>
#include <vector>

#include <antares/mersenne-twister/mersenne-twister.h>
#include <antares/study/parameters.h>

namespace Antares::Data
{

class Study;

enum RangeLimitsIndex
{
    rangeBegin = 0,
    rangeEnd,
    rangeCount,
    rangeMax
};

struct StudyRangeLimits
{
public:
    /*!
    ** \brief Dynamic bound-checking (debug only)
    */
    void checkIntegrity() const;

    //! Hours
    uint hour[rangeMax];
    //! Day
    uint day[rangeMax];
    //! Week
    uint week[rangeMax];
    //! Month
    uint month[rangeMax];
    //! Year
    uint year[rangeMax];

}; // class StudyRangeLimits

/*!
** \brief Runtime informations
** \ingroup runtimedata
*/
class StudyRuntimeInfos
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    StudyRuntimeInfos();
    /*!
    ** \brief Destructor
    */
    ~StudyRuntimeInfos();
    //@}

    /*!
    ** \brief Reset internal data according a given study
    */
    bool loadFromStudy(Study& study);

    void initializeRandomNumberGenerators(const Parameters& parameters);

public:
    //! The number of years to process
    uint nbYears;

    //! Range limits
    StudyRangeLimits rangeLimits;

    //! Mode of the simulation
    SimulationMode mode;

    //! The number of interconnections
    uint interconnectionsCount() const;
    //! All interconnections
    std::vector<AreaLink*> areaLink;

    //! Random numbers generators
    MersenneTwister random[seedMax];

    //! Total
    uint thermalPlantTotalCount;
    uint thermalPlantTotalCountMustRun;

    uint shortTermStorageCount = 0;
    uint shortTermStorageCumulativeConstraintCount = 0;

    //! Override enable/disable TS generation per cluster
    bool thermalTSRefresh = false;

    /*!
    ** \brief The number of simulation days per month
    */
    uint simulationDaysPerMonth[12];

    /*!
    ** \brief The number of simulation days per week
    */
    uint simulationDaysPerWeek[53];

    /*!
    ** \brief Flag to know if at least one error occured during the quadratic optimization
    **
    ** In this case, error on NaN should be disabled
    */
    bool quadraticOptimizationHasFailed;

    std::vector<std::vector<double>> transitMoyenInterconnexionsRecalculQuadratique;

private:
    void initializeRangeLimits(const Study& study, StudyRangeLimits& limits);
    void removeDisabledShortTermStorageClustersFromSolverComputations(Study& study);
    void removeAllRenewableClustersFromSolverComputations(Study& study);
    void disableAllFilters(Study& study);
    void checkThermalTSGeneration(Study& study);
}; // struct StudyRuntimeInfos

#ifdef NDEBUG
inline void StudyRangeLimits::checkIntegrity() const
{
}
#endif

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__
