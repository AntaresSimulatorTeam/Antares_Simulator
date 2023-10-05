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
#include <vector>
#include "../../fwd.h"

namespace Antares::Data
{

class TSNumbers
{
public:
    uint32_t get(uint32_t year) const;

private:
    std::vector<uint32_t> tsNumbers_;
};

class TimeSeries
{
public:
    TimeSeries(const TSNumbers& tsNumbers);
    double getCoefficient(uint32_t year, uint32_t hourInYear) const;
    double* getColumn(uint32_t year) const;

private:
    Matrix<double, int32_t> coefficients;
    const TSNumbers& tsNumbers;
};

/*!
** \brief Data series (Common)
*/
class DataSeriesCommon
{
public:
    using AllYears = Matrix<double>;
    using SingleYear = AllYears::ColumnType;

    bool forceReload(bool reload = false) const;

    void markAsModified() const;

    uint64_t memoryUsage() const;

    double getAvailablePower(unsigned int hour, unsigned int year) const;
    const SingleYear& getAvailablePowerYearly(unsigned int year) const;

    /*!
    ** \brief Series (MW)
    **
    ** Merely a matrix of TimeSeriesCount * 8760 values
    */
    AllYears timeSeries;

    /*!
    ** \brief Monte-Carlo
    */
    Matrix<uint32_t> timeseriesNumbers;

private:
    uint getSeriesIndex(unsigned int year) const;

}; // class DataSeriesCommon

} // namespace Antares::Data
#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_TIMESERIES_H__ */
