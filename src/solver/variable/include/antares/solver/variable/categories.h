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
#ifndef __SOLVER_VARIABLE_CATEGORIES_H__
#define __SOLVER_VARIABLE_CATEGORIES_H__

#include <antares/antares/constants.h>

namespace Antares::Solver::Variable::Category
{
namespace DataLevel
{
//! Data that belong to a single area
constexpr uint8_t area = 1;
//! Data that belong to a thermal dispatchable cluster
constexpr uint8_t thermalAggregate = 2;
//! Data that belong to a link
constexpr uint8_t link = 4;
//! Data that belong to a set
constexpr uint8_t setOfAreas = 8;
// Data belonging to a binding constraint
constexpr uint8_t bindingConstraint = 16;
//! The maximum available level
constexpr uint8_t maxDataLevel = 16;
} // namespace DataLevel

namespace FileLevel
{
//! Values of physical variables
constexpr uint8_t va = 1;
//! Reference numbers
constexpr uint8_t id = 2;
//! Detailed values regarding thermal generation
constexpr uint8_t de = 4;
//! Detailed values regarding RES generation
constexpr uint8_t de_res = 8;
//! Detailed values regarding binding constraints
constexpr uint8_t bc = 16;
//! Detailed values regarding short term storage
constexpr uint8_t de_sts = 32;
//! The maximum available value
constexpr uint8_t maxFileLevel = 32;
//! All file level
constexpr uint8_t allFile = va | id | de | de_res | bc | de_sts;
}; // namespace FileLevel

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
    return (fileLevel != FileLevel::id) ? 2u : 0u;
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
    switch (dataLevel)
    {
        using namespace DataLevel;
    case area:
        out += "area";
        break;
    case thermalAggregate:
        out += "thermal";
        break;
    case link:
        out += "link";
        break;
    case setOfAreas:
        out += "set of areas";
        break;
    default:
        out += NULL;
    };
}

template<class StreamT>
inline void FileLevelToStreamShort(StreamT& out, int fileLevel)
{
    switch (fileLevel)
    {
        using namespace FileLevel;
    case va:
        out += "va";
        break;
    case id:
        out += "id";
        break;
    case de:
        out += "de";
        break;
    case de_res:
        out += "res";
        break;
    case bc:
        out += "bc";
        break;
    case de_sts:
        out += "sts";
        break;
    default:
        out += NULL;
    }
}

template<class StreamT>
inline void FileLevelToStream(StreamT& out, int fileLevel)
{
    switch (fileLevel)
    {
        using namespace FileLevel;
    case va:
        out += "values";
        break;
    case id:
        out += "id";
        break;
    case de:
        out += "details";
        break;
    case de_res:
        out += "details-res";
        break;
    case bc:
        out += "binding-constraints";
        break;
    case de_sts:
        out += "details-STstorage";
        break;
    default:
        out += NULL;
    }
}

template<class StreamT>
inline void PrecisionLevelToStream(StreamT& out, int precisionLevel)
{
    switch (precisionLevel)
    {
    case hourly:
        out += "hourly";
        break;
    case daily:
        out += "daily";
        break;
    case weekly:
        out += "weekly";
        break;
    case monthly:
        out += "monthly";
        break;
    case annual:
        out += "annual";
        break;
    default:
        out += NULL;
    }
}

} // namespace Antares::Solver::Variable::Category

#endif // __SOLVER_VARIABLE_CATEGORIES_H__
