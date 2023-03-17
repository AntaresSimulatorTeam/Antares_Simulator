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
    return true;
}

bool Series::loadFromFolder(const std::string& folder)
{
    logs.notice() << "series::loadfromfolder";
    bool ret = true;
    ret = loadFile(folder, "PMAX-injection.txt") && ret;
    return ret;
}

bool Series::loadFile(const std::string& folder, const std::string filename)
{
    std::string path(folder + filename);

    std::ifstream file;
    file.open(path);

    logs.notice() << path;

    if ((file.rdstate() & std::ifstream::failbit ) != 0)
        return false;

    logs.notice() << "File OK";

    std::vector<double> vect = *getVectorWithName(filename);
    vect.resize(8760);

    std::string line;
    try
    {
        size_t count = 0;
        while (getline(file, line))
        {
            double d = std::stod(line);
            vect.push_back(d);
            logs.notice() << count++ << " : " << d;
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

    logs.error() << "Filename: " << name << " not found for series";
    return nullptr;
}

} // namespace Antares::Data::ShortTermStorage
