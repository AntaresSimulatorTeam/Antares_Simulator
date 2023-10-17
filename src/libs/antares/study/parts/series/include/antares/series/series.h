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
#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_TIMESERIES_H__

#include <antares/array/matrix.h>

namespace Antares::Data
{

class TimeSeries
{
public:
    using TSNumbers = Matrix<uint32_t>;
    using TS = Matrix<double>;

    TimeSeries(TSNumbers& tsNumbers);
    /*!
     ** \brief Load series from a file
     **
     ** \param d Data series
     ** \param areaID The ID of the area associated to the data series
     ** \param folder The source folder
     ** \param filename The filename of the series
     ** \return A non-zero value if the operation succeeded, 0 otherwise
     */
    bool timeSeriesLoadFromFolder(const std::string& path,
                                  Matrix<>::BufferType dataBuffer,
                                  const bool average);
    /*!
     ** \brief Save time series to a file
     ** \ingroup windseries
     **
     ** \param areaID The ID of the area associated to the data series
     ** \param folder The target folder
     ** \param filename The filename of the series
     ** \return A non-zero value if the operation succeeded, 0 otherwise
     */
    int timeSeriesSaveToFolder(const AreaName& areaID, const std::string& folder,
                               const std::string& prefix) const;

    double getCoefficient(uint32_t year, uint32_t hourInYear) const;
    const double* getColumn(uint32_t year) const;
    uint32_t getSeriesIndex(uint32_t year) const;

    void reset();

    bool forceReload(bool reload = false) const;
    void markAsModified() const;
    uint64_t memoryUsage() const;

    TS timeSeries;
    TSNumbers& timeseriesNumbers;

    static const double emptyColumn[HOURS_PER_YEAR];
};

} // namespace Antares::Data
#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_TIMESERIES_H__ */
