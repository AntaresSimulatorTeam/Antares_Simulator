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
#ifndef __SOLVER_VARIABLE_CATEGORIES_H__
#define __SOLVER_VARIABLE_CATEGORIES_H__

#include "constants.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Category
{
enum DataLevel
{
    //! Data that do not belong to any area
    standard = 1,
    //! Data that belong to a single area
    area = 2,
    //! Data that belong to a thermal dispatchable cluster
    thermalAggregate = 4,
    //! Data that belong to a link
    link = 8,
    //! Data that belong to a set
    setOfAreas = 16,
    //! The maximum available level
    maxDataLevel = 16,
    //! All data level
    allDataLevel = standard | area | thermalAggregate | link | setOfAreas
};

enum File
{
    //! Values of physical variables
    va = 1,
    //! Reference numbers
    id = 2,
    //! Detailed values regarding thermal generation
    de = 4,
    //! Adequacy results not taking into account the interconnection capacities
    is = 8,
    //! Adequacy results taking into account the actually available interconnection capacities
    cn = 16,
    //! Annual adequacy results
    mc = 32,
    //! The maximum available value
    maxFileLevel = 32,
    //! All file level
    allFile = va | id | de | is | cn | mc,
};

enum Precision
{
    //! hour
    hourly = 1,
    //! Daily
    daily = 2,
    //! Week
    weekly = 4,
    //! Month
    monthly = 8,
    //! Year
    annual = 16,
    //! All precisions
    all = hourly | daily | weekly | monthly | annual,
};

/*!
** \brief Column management per variable
*/
enum ColumnManagement
{
    //! Special value for dynamic management (the number of columns can only be known at runtime)
    dynamicColumns = -1,
    //! No column (means 'no data')
    noColumn = 0,
    //! A single column for the variable (the default)
    singleColumn = 1,
};

/*!
** \brief Digest levels
*/
enum Digest
{
    //! No digest
    digestNone = 0,
    //! Only all years
    digestAllYears = 1,
    //! Flow (linear)
    digestFlowLinear = 2,
    //! Flow (Quadratic)
    digestFlowQuad = 4,
};

/*!
** \brief Spatial clusters (bitwise)
*/
enum SpatialAggregate
{
    //! No spatial cluster (default)
    noSpatialAggregate = 0,
    //! The sum must be used
    spatialAggregateSum = 1,
    //! The maximum must be used
    spatialAggregateMax = 2,
    //! Average between areas
    spatialAggregateAverage = 4,
    //! Sum + 1
    spatialAggregateSumThen1IfPositive = 8,
    //!
    spatialAggregateOr = 16,
};

enum SpatialAggregateMode
{
    //! Compute the aggregate each year
    spatialAggregateEachYear = 1,
    //! Compute the aggregate once, at the end of the simulation
    spatialAggregateOnce = 2,
};

enum SpatialAggregatePostProcessing
{
    spatialAggregatePostProcessingPrice = 1,
};

/*!
** \brief Hard limit for the decimal precision according a given File precision level
*/
static inline uint MaxDecimalPrecision(uint fileLevel)
{
    return (fileLevel != id) ? 2u : 0u;
}

template<int Index, int Limit>
struct NextLevel
{
    enum
    {
        next = (Index != Limit) ? Index * 2 : 0,
    };
};

template<class StreamT>
inline void DataLevelToStream(StreamT& out, int dataLevel)
{
    static const char* const values[] = {
      NULL,
      "group", //  1 - standard
      "area",  //  2 - area
      NULL,
      "thermal", //  4 - thermal
      NULL,
      NULL,
      NULL,
      "link", //  8 - link
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "set of areas" // 16 - set of areas
    };
    assert(dataLevel <= 16);
    out += values[dataLevel];
}

template<class StreamT>
inline void FileLevelToStreamShort(StreamT& out, int fileLevel)
{
    static const char* const values[] = {
      NULL, "va", "id", NULL, "de", NULL, NULL, NULL, "is", NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, "cn", NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "mc",
    };
    assert(fileLevel <= 32);
    out += values[fileLevel];
}

template<class StreamT>
inline void FileLevelToStream(StreamT& out, int fileLevel)
{
    static const char* const values[] = {
      NULL, "values", "id", NULL, "details", NULL, NULL, NULL,           "without-network",
      NULL, NULL,     NULL, NULL, NULL,      NULL, NULL, "with-network", NULL,
      NULL, NULL,     NULL, NULL, NULL,      NULL, NULL, NULL,           NULL,
      NULL, NULL,     NULL, NULL, NULL,      "mc",
    };
    assert(fileLevel <= 32);
    out += values[fileLevel];
}

template<class StreamT>
inline void PrecisionLevelToStream(StreamT& out, int precisionLevel)
{
    static const char* const values[] = {NULL,
                                         "hourly",
                                         "daily",
                                         NULL,
                                         "weekly",
                                         NULL,
                                         NULL,
                                         NULL,
                                         "monthly",
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         "annual"};
    assert(precisionLevel <= 16);
    out += values[precisionLevel];
}

template<int PrecisionLevel>
struct MaxRowCount
{
    enum
    {
        value = 0
    };
};
template<>
struct MaxRowCount<hourly>
{
    enum
    {
        value = maxHoursInAYear
    };
};
template<>
struct MaxRowCount<daily>
{
    enum
    {
        value = maxDaysInAYear
    };
};
template<>
struct MaxRowCount<weekly>
{
    enum
    {
        value = maxWeeksInAYear
    };
};
template<>
struct MaxRowCount<monthly>
{
    enum
    {
        value = maxMonths
    };
};
} // namespace Category
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_CATEGORIES_H__
