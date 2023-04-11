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

#include <optional>
#include <fstream>

#include "series.h"

namespace Antares::Data::ShortTermStorage
{


bool Series::loadFromFolder(const std::string& folder)
{
    bool ret = true;
#define SEP Yuni::IO::Separator
    ret = loadFile(folder + SEP + "PMAX-injection.txt", maxInjectionModulation) && ret;
    ret = loadFile(folder + SEP + "PMAX-withdrawal.txt", maxWithdrawalModulation) && ret;
    ret = loadFile(folder + SEP + "inflows.txt", inflows) && ret;
    ret = loadFile(folder + SEP + "lower-rule-curve.txt", lowerRuleCurve) && ret;
    ret = loadFile(folder + SEP + "upper-rule-curve.txt", upperRuleCurve) && ret;
#undef SEP
    return ret;
}

bool loadFile(const std::string& path, std::vector<double>& vect)
{
    logs.debug() << "  :: loading file " << path;

    vect.reserve(HOURS_PER_YEAR);

    std::ifstream file;
    file.open(path);

    if (!file)
    {
        logs.debug() << "File not found: " << path;
        return true;
    }

    unsigned int lineCount = 0;
    std::string line;
    try
    {
        while (getline(file, line) && lineCount < HOURS_PER_YEAR)
        {
            double d = std::stod(line);
            vect.push_back(d);
            lineCount++;
        }
        if (lineCount < HOURS_PER_YEAR)
        {
            logs.warning() << "File too small: " << path;
            return false;
        }
    }
    catch (const std::ios_base::failure& ex)
    {
        logs.error() << "Failed reading file: " << path << " (I/O error)";
        return false;
    }
    catch (const std::invalid_argument& ex)
    {
        logs.error() << "Failed reading file: " << path << " conversion to double failed at line "
            << lineCount + 1 << "  value: " << line;
        return false;
    }
    catch (const std::out_of_range& ex)
    {
        logs.error() << "Failed reading file: " << path << " value is out of bounds at line "
            << lineCount + 1 << "  value: " << line;
        return false;
    }
    return true;
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

bool Series::validate() const
{
    return validateSizes() && validateMaxInjection() && validateMaxWithdrawal()
        && validateRuleCurves();
}

static bool checkVectBetweenZeroOne(const std::vector<double>& v, const std::string& name )
{
    if(!std::all_of(v.begin(), v.end(), [](double d){ return (d >= 0.0 && d <= 1.0); }))
    {
        logs.warning() << "Values for " << name << " series should be between 0 and 1";
        return false;
    }
    return true;
}

bool Series::validateSizes() const
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

bool Series::validateMaxInjection() const
{
    return checkVectBetweenZeroOne(maxInjectionModulation, "PMAX injection");
}

bool Series::validateMaxWithdrawal() const
{
    return checkVectBetweenZeroOne(maxWithdrawalModulation, "PMAX withdrawal");
}

bool Series::validateRuleCurves() const
{
    if (!validateUpperRuleCurve() || !validateLowerRuleCurve())
        return false;

    for (int i = 0; i < HOURS_PER_YEAR; i++)
    {
        if (lowerRuleCurve[i] > upperRuleCurve[i])
        {
            logs.warning() << "Lower rule curve greater than upper at line: " << i + 1;
            return false;
        }
    }
    return true;
}

bool Series::validateUpperRuleCurve() const
{
    return checkVectBetweenZeroOne(upperRuleCurve, "upper rule curve");
}

bool Series::validateLowerRuleCurve() const
{
    return checkVectBetweenZeroOne(maxInjectionModulation, "lower rule curve");
}

bool Series::validateInitialLevelSimplex(bool simplexIsWeek, std::optional<double> level,
        unsigned int cycle, unsigned int startHour, unsigned int endHour)
{
    if (startHour > endHour)
        endHour += HOURS_PER_YEAR;

    unsigned int simulationHour = (simplexIsWeek) ? 168 : 24;

    if (level.has_value())
        return checkLevelValue(level.value(), cycle, startHour, endHour) &&
                checkLevelValue(level.value(), simulationHour, startHour, endHour);
    else
        return checkLevelInterval(cycle, startHour, endHour) &&
                checkLevelInterval(simulationHour, startHour, endHour);

    return true;
}

bool Series::checkLevelValue(double level, unsigned int cycleDuration, unsigned int startHour, unsigned int endHour) const
{
    for (unsigned int h = startHour; h < endHour; h += cycleDuration)
    {
        unsigned int realHour = h % HOURS_PER_YEAR;
        logs.notice() << "h: " << realHour << " lrc: " << lowerRuleCurve[realHour] <<  " level : " << level <<
            " urc: " << upperRuleCurve[realHour];

        if (upperRuleCurve[realHour] < level ||
                lowerRuleCurve[realHour] > level)
        {
            logs.warning() << "Error at line: " << realHour + 1 << " for sts series upper or  " <<
                "lower rule curves, initial level is not between those values";

            return false;
        }
    }
    return true;
}

bool Series::checkLevelInterval(unsigned int cycleDuration, unsigned int startHour, unsigned int endHour) const
{
    double min = lowerRuleCurve[startHour];
    double max = upperRuleCurve[startHour];

    for (unsigned int h = startHour + cycleDuration; h < endHour; h += cycleDuration)
    {
        unsigned int realHour = h % HOURS_PER_YEAR;
        if (upperRuleCurve[realHour] < min ||
                lowerRuleCurve[realHour] > max)
        {
            logs.warning() << "Error at line: " << realHour + 1 << " for sts series upper or  " <<
                "lower rule curves, values at the start of the cycle are outside of those values";

            return false;
        }
    }
    return true;
}

} // namespace Antares::Data::ShortTermStorage
