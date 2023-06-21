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
#ifndef __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__
#define __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__

#include <string>
#include <vector>
#include "../study.h"
#include "../../mersenne-twister/mersenne-twister.h"

namespace Antares::Data
{

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

class BindingConstraintRTI
{
public:
    BindingConstraint::Type type;
    char operatorType;
    uint filterYearByYear_ = filterAll;
    uint filterSynthesis_ = filterAll;

    uint linkCount = 0;
    std::vector<double> linkWeight;
    std::vector<int> linkOffset;
    std::vector<long> linkIndex;

    uint clusterCount = 0;
    std::vector<double> clusterWeight;
    std::vector<int> clusterOffset;
    std::vector<long> clusterIndex;
    std::vector<long> clustersAreaIndex;
    std::string name;
    std::string group;
    Matrix<> rhsTimeSeries;

    const Matrix<>& RHSTimeSeries() const { return rhsTimeSeries; }
};

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
    StudyRuntimeInfos(uint nbYearsParallel);
    /*!
    ** \brief Destructor
    */
    ~StudyRuntimeInfos();
    //@}

    /*!
    ** \brief Reset internal data according a given study
    */
    bool loadFromStudy(Study& study);

    std::vector<uint> getIndicesForInequalityBindingConstraints() const;

public:
    //! The number of years to process
    uint nbYears;
    //! How many hours per year
    uint nbHoursPerYear; // default: 8760
    //! Number of days per year
    uint nbDaysPerYear;
    //! Number
    uint nbMonthsPerYear;

    //! Alias for General data
    Parameters* parameters;

    //! Range limits
    StudyRangeLimits rangeLimits;

    //! Mode of the simulation
    StudyMode mode;

    //! The number of interconnections
    uint interconnectionsCount() const;
    //! All interconnections
    std::vector<AreaLink*> areaLink;

    //! Random numbers generators
    MersenneTwister random[seedMax];

    /*!
    ** \brief The index to use when retrieving the time-series numbers
    **
    ** To allow the drop of the years with no solution, we can not fully rely
    ** on the current year. So we have to maintain a rotating index (0..nbYears)
    */
    uint* timeseriesNumberYear;

    std::vector<BindingConstraintRTI> bindingConstraints;

    //! Total
    uint thermalPlantTotalCount;
    uint thermalPlantTotalCountMustRun;

    uint shortTermStorageCount = 0;

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

private:
    void initializeBindingConstraints(BindingConstraintsList& list);
    void initializeRangeLimits(const Study& study, StudyRangeLimits& limits);
    //! Prepare all thermal clusters in 'must-run' mode
    void initializeThermalClustersInMustRunMode(Study& study) const;
    void removeDisabledThermalClustersFromSolverComputations(Study& study);
    void removeDisabledRenewableClustersFromSolverComputations(Study& study);
    void removeAllRenewableClustersFromSolverComputations(Study& study);
    void disableAllFilters(Study& study);
    void checkThermalTSGeneration(Study& study);
}; // struct StudyRuntimeInfos

/*!
** \brief Get the size (bytes) occupied in memory by a StudyRuntimeInfos structure
** \ingroup runtimedata
*/
void StudyRuntimeInfosEstimateMemoryUsage(StudyMemoryUsage& study);

} // namespace Antares::Data

#include "runtime.hxx"

#endif // __ANTARES_LIBS_STUDY_RUNTIME_RUNTIME_INFOS_H__
