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

#include <antares/logs.h>
#include <fstream>

#include "series.h"

namespace Antares::Data::ShortTermStorage
{

bool Series::validate() const
{
    logs.notice() << "maxWithdrawal" << maxWithdrawal.size();
    if (maxInjection.size() != VECTOR_SERIES_SIZE ||
        maxWithdrawal.size() != VECTOR_SERIES_SIZE ||
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

    ret = loadFile(folder, "PMAX-injection.txt", maxInjection) && ret;
    ret = loadFile(folder, "PMAX-withdrawal.txt", maxWithdrawal) && ret;
    ret = loadFile(folder, "inflow.txt", inflows) && ret;
    ret = loadFile(folder, "lower-rule-curve.txt", lowerRuleCurve) && ret;
    ret = loadFile(folder, "upper-rule-curve.txt", upperRuleCurve) && ret;

    return ret;
}

bool Series::loadFile(const std::string& folder, const std::string& filename, std::vector<double>& vect)
{
    std::string path(folder + filename);
    logs.debug() << "Loading file " << path;

    vect.reserve(VECTOR_SERIES_SIZE);

    std::ifstream file;
    file.open(path);

    if ((file.rdstate() & std::ifstream::failbit ) != 0)
        return false;

    std::string line;
    try
    {
        while (getline(file, line))
        {
            double d = std::stod(line);
            vect.push_back(d);
        }
    }
    catch (const std::exception&)
    {
        logs.error() << "Failed reading file: " << path;
        return false;
    }

    return true;
}

} // namespace Antares::Data::ShortTermStorage
