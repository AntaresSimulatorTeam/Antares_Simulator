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
#ifndef __ANTARES_LIBS_STUDY_PARTS_SOLAR_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_PARTS_SOLAR_TIMESERIES_H__

#include <antares/array/matrix.h>
#include "../../fwd.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Data series (Solar)
*/
class DataSeriesSolar
{
public:
    void estimateMemoryUsage(StudyMemoryUsage&) const;

    bool forceReload(bool reload = false) const;

    void markAsModified() const;

public:
    /*!
    ** \brief Time series (MW)
    **
    ** Merely a matrix of TimeSeriesCount * 8760 values
    */
    Matrix<double, Yuni::sint32> timeSeries;

    /*!
    ** \brief Monte-Carlo
    */
    Matrix<Yuni::uint32> timeseriesNumbers;

}; /* class DataSeriesSolar */

/*!
** \brief Solar wind data series from a file
** \ingroup windseries
**
** \param d Data series
** \param areaID The ID of the area associated to the data series
** \param folder The source folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int DataSeriesSolarLoadFromFolder(Study& s,
                                  DataSeriesSolar* d,
                                  const AreaName& areaID,
                                  const char folder[]);

/*!
** \brief Save wind data series from a file
** \ingroup windseries
**
** \param d Data series
** \param areaID The ID of the area associated to the data series
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int DataSeriesSolarSaveToFolder(DataSeriesSolar* d, const AreaName& areaID, const char folder[]);

/*!
** \brief Get the size (bytes) in memory occupied by a `DataSeriesSolar` structure
*/
Yuni::uint64 DataSeriesSolarMemoryUsage(DataSeriesSolar* w);

} // namespace Data
} // namespace Antares

#include "series.hxx"

#endif /* __ANTARES_LIBS_STUDY_PARTS_SOLAR_TIMESERIES_H__ */
