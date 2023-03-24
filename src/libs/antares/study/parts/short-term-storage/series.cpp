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
#include <yuni/io/file.h>
#include <antares/logs.h>
#include <antares/array/array1d.h>
#include <fstream>

#include "series.h"

#define VECTOR_SERIES_SIZE 8760

namespace Antares::Data::ShortTermStorage
{

bool Series::validate() const
{
    if (maxInjectionModulation.size() != VECTOR_SERIES_SIZE ||
        maxWithdrawalModulation.size() != VECTOR_SERIES_SIZE ||
        inflows.size() != VECTOR_SERIES_SIZE ||
        lowerRuleCurve.size() != VECTOR_SERIES_SIZE ||
        upperRuleCurve.size() != VECTOR_SERIES_SIZE)
    {
        logs.warning() << "Size of series for short term storage is wrong";
        return false;
    }
    return true;
}

bool Series::loadFromFolder(const std::string& folder)
{
    bool ret = true;

    ret = loadVector(folder + "PMAX-injection.txt", maxInjectionModulation) && ret;
    ret = loadVector(folder + "PMAX-withdrawal.txt", maxWithdrawalModulation) && ret;
    ret = loadVector(folder + "inflow.txt", inflows) && ret;
    ret = loadVector(folder + "lower-rule-curve.txt", lowerRuleCurve) && ret;
    ret = loadVector(folder + "upper-rule-curve.txt", upperRuleCurve) && ret;

    return ret;
}

bool Series::loadVector(const std::string& path, std::vector<double>& vect)
{
    if (!Yuni::IO::File::Exists(path))
        return true;

    vect.resize(VECTOR_SERIES_SIZE);
    return Array1DLoadFromFile(path.c_str() , &vect[0], VECTOR_SERIES_SIZE);
}

void Series::fillDefaultSeriesIfEmpty()
{
    auto fillIfEmpty = [](std::vector<double> & v,
                          double value)
                          {
                              if (v.empty())
                                  v.resize(VECTOR_SERIES_SIZE, value);
                          };

    fillIfEmpty(maxInjectionModulation, 1.0);
    fillIfEmpty(maxWithdrawalModulation, 1.0);
    fillIfEmpty(inflows, 1.0);
    fillIfEmpty(lowerRuleCurve, 0.0);
    fillIfEmpty(upperRuleCurve, 1.0);
}

} // namespace Antares::Data::ShortTermStorage
