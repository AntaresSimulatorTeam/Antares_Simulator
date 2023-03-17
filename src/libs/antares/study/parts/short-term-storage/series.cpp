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

/* Series::Series() */
/* { */
/*     logs.notice() << "constructor begins"; */
/*     seriesNameMap.try_emplace("PMAX-injection.txt", &maxInjection); */
/*     seriesNameMap.try_emplace("PMAX-withdrawal.txt", &maxWithdrawal); */
/*     seriesNameMap.try_emplace("inflow.txt", &inflows); */
/*     seriesNameMap.try_emplace("lower-rule-curve.txt", &lowerRuleCurve); */
/*     seriesNameMap.try_emplace("upper-rule-curve.txt.txt", &upperRuleCurve); */
/*     logs.notice() << "constructor end"; */
/* } */

bool Series::validate() const
{
    return true;
}

bool Series::loadFromFolder(const std::string& folder)
{
    bool ret = true;
    /* for (auto& [name, vect]: seriesNameMap) */
    /*     ret = loadFile(folder, name, *vect) && ret; */
    ret = ret && loadFile(folder, "PMAX-injection.txt", *getVectorWithName("PMAX-injection.txt"));
    ret = ret && loadFile(folder, "PMAX-withdrawal.txt", *getVectorWithName("PMAX-withdrawal.txt"));
    ret = ret && loadFile(folder, "inflow.txt", *getVectorWithName("inflow.txt"));
    ret = ret && loadFile(folder, "lower-rule-curve.txt", *getVectorWithName("lower-rule-curve.txt"));
    ret = ret && loadFile(folder, "upper-rule-curve.txt", *getVectorWithName("upper-rule-curve.txt"));
    return ret;
}

bool Series::loadFile(const std::string& folder, const std::string filename, std::vector<double>& vect)
{
    std::string path(folder + filename);

    std::ifstream file;
    file.open(path);

    logs.notice() << path;

    if ((file.rdstate() & std::ifstream::failbit ) != 0)
        return false;

    logs.notice() << "File OK";

    vect.reserve(8760);

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

//TODO replace this function with a map
std::vector<double> *Series::getVectorWithName(const std::string& name)
{
    if (name == "PMAX-injection.txt")
        return &maxInjection;
    if (name == "PMAX-withdrawal.txt")
        return &maxWithdrawal;
    if (name == "inflow.txt")
        return &inflows;
    if (name == "lower-rule-curve.txt")
        return &lowerRuleCurve;
    if (name == "upper-rule-curve.txt")
        return &upperRuleCurve;

    logs.error() << "Filename: " << name << " not found for series";
    return nullptr;
}

} // namespace Antares::Data::ShortTermStorage
