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
    if (maxInjectionModulation.size() != VECTOR_SERIES_SIZE
        || maxWithdrawalModulation.size() != VECTOR_SERIES_SIZE
        || inflows.size() != VECTOR_SERIES_SIZE || lowerRuleCurve.size() != VECTOR_SERIES_SIZE
        || upperRuleCurve.size() != VECTOR_SERIES_SIZE)
    {
        logs.warning() << "Size of series for short term storage is wrong";
        return false;
    }

    auto checkVectPositive = [](std::vector<double> v) {
        return std::count_if(v.begin(), v.end(), [](double d){ return d >= 0.0; });
    };
    if (!checkVectPositive(maxInjectionModulation) || !checkVectPositive(maxWithdrawalModulation))
    {
        logs.warning() << "Values for PMAX series should be positive";
        return false;
    }

    auto checkVectBetween = [](std::vector<double> v) {
        return std::count_if(v.begin(), v.end(), [](double d){ return (d >= 0.0 && d <= 1.0); });
    };
    if (!checkVectBetween(lowerRuleCurve) || !checkVectBetween(upperRuleCurve))
    {
        logs.warning() << "Values for rule curve series should be between 0 and 1";
        return false;
    }

    return true;
}

/* bool Series::loadFromFolder(const std::string& folder) */
/* { */
/*     bool ret = true; */
/* #define SEP Yuni::IO::Separator */
/*     ret = loadVector(folder + SEP + "PMAX-injection.txt", maxInjectionModulation) && ret; */
/*     ret = loadVector(folder + SEP + "PMAX-withdrawal.txt", maxWithdrawalModulation) && ret; */
/*     ret = loadVector(folder + SEP + "inflows.txt", inflows) && ret; */
/*     ret = loadVector(folder + SEP + "lower-rule-curve.txt", lowerRuleCurve) && ret; */
/*     ret = loadVector(folder + SEP + "upper-rule-curve.txt", upperRuleCurve) && ret; */
/* #undef SEP */
/*     return ret; */
/* } */

bool Series::loadFromFolder(const std::string& folder)
{
    bool ret = true;
#define SEP Yuni::IO::Separator
    ret = loadFile(folder + SEP + "PMAX-injection.txt", maxInjectionModulation, VECTOR_SERIES_SIZE) && ret;
    ret = loadFile(folder + SEP + "PMAX-withdrawal.txt", maxWithdrawalModulation, VECTOR_SERIES_SIZE) && ret;
    ret = loadFile(folder + SEP + "inflows.txt", inflows, VECTOR_SERIES_SIZE) && ret;
    ret = loadFile(folder + SEP + "lower-rule-curve.txt", lowerRuleCurve, VECTOR_SERIES_SIZE) && ret;
    ret = loadFile(folder + SEP + "upper-rule-curve.txt", upperRuleCurve, VECTOR_SERIES_SIZE) && ret;
#undef SEP
    return ret;
}

bool Series::loadFile(const std::string& path, std::vector<double>& vect, unsigned int size)
{
    logs.debug() << "Loading file " << path;

    vect.reserve(size);

    std::ifstream file;
    file.open(path);

    if (!file)
    {
        logs.debug() << "File not found: " << path;
        return true;
    }

    std::string line;
    try
    {
        unsigned int count = 0;
        while (getline(file, line) && count < size)
        {
            double d = std::stod(line);
            vect.push_back(d);
            count++;
        }
        if (count < size)
            return false;
    }
    catch (const std::exception&)
    {
        logs.error() << "Failed reading file: " << path;
        return false;
    }
    return true;
}


bool Series::loadVector(const std::string& path, std::vector<double>& vect)
{
    if (!Yuni::IO::File::Exists(path))
        return true;

    vect.resize(VECTOR_SERIES_SIZE);
    return Array1DLoadFromFile(path.c_str(), vect.data(), VECTOR_SERIES_SIZE);
}

void Series::fillDefaultSeriesIfEmpty()
{
    auto fillIfEmpty = [](std::vector<double>& v, double value) {
        if (v.empty())
            v.resize(VECTOR_SERIES_SIZE, value);
    };

    fillIfEmpty(maxInjectionModulation, 1.0);
    fillIfEmpty(maxWithdrawalModulation, 1.0);
    fillIfEmpty(inflows, 0.0);
    fillIfEmpty(lowerRuleCurve, 0.0);
    fillIfEmpty(upperRuleCurve, 1.0);
}

} // namespace Antares::Data::ShortTermStorage
