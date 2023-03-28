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
#include <antares/constants.h>
#include <antares/array/array1d.h>

#include "series.h"

namespace Antares::Data::ShortTermStorage
{
bool Series::validate() const
{
    if (maxInjectionModulation.size() != HOURS_PER_YEAR
        || maxWithdrawalModulation.size() != HOURS_PER_YEAR || inflows.size() != HOURS_PER_YEAR
        || lowerRuleCurve.size() != HOURS_PER_YEAR || upperRuleCurve.size() != HOURS_PER_YEAR)
    {
        logs.warning() << "Size of series for short term storage is wrong";
        return false;
    }
    return true;
}

bool Series::loadFromFolder(const std::string& folder)
{
    bool ret = true;
#define SEP Yuni::IO::Separator
    ret = loadVector(folder + SEP + "PMAX-injection.txt", maxInjectionModulation) && ret;
    ret = loadVector(folder + SEP + "PMAX-withdrawal.txt", maxWithdrawalModulation) && ret;
    ret = loadVector(folder + SEP + "inflows.txt", inflows) && ret;
    ret = loadVector(folder + SEP + "lower-rule-curve.txt", lowerRuleCurve) && ret;
    ret = loadVector(folder + SEP + "upper-rule-curve.txt", upperRuleCurve) && ret;
#undef SEP
    return ret;
}

bool Series::loadVector(const std::string& path, std::vector<double>& vect)
{
    if (!Yuni::IO::File::Exists(path))
        return true;

    vect.resize(HOURS_PER_YEAR);
    return Array1DLoadFromFile(path.c_str(), vect.data(), HOURS_PER_YEAR);
}

void Series::fillDefaultSeriesIfEmpty()
{
    auto fillIfEmpty = [](std::vector<double>& v, double value) {
        if (v.empty())
            v.resize(HOURS_PER_YEAR, value);
    };

    fillIfEmpty(maxInjectionModulation, 1.0);
    fillIfEmpty(maxWithdrawalModulation, 1.0);
    fillIfEmpty(inflows, 0.0);
    fillIfEmpty(lowerRuleCurve, 0.0);
    fillIfEmpty(upperRuleCurve, 1.0);
}

} // namespace Antares::Data::ShortTermStorage
