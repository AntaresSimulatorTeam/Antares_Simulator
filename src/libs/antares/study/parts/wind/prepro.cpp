/*
** Copyright 2007-2018 RTE
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

#include <yuni/yuni.h>
#include "prepro.h"
#include <yuni/io/directory.h>
#include "../../study.h"
#include "../../../logs.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
namespace Wind
{
Prepro::Prepro() : xcast(timeSeriesWind)
{
}

Prepro::~Prepro()
{
}

bool Prepro::loadFromFolder(Study& study, const AnyString& folder)
{
    return (study.header.version <= 320) ? internalLoadFormatBefore33(study, folder)
                                         : xcast.loadFromFolder(study, folder);
}

bool Prepro::internalLoadFormatBefore33(Study& study, const AnyString& folder)
{
    // Reset xcast
    xcast.resetToDefaultValues();

    // Temporary data
    String filename;
    Matrix<>::BufferType tmpdata;
    Matrix<float> m;
    bool ret = true;

    enum
    {
        mtrxOption = Matrix<>::optFixedSize | Matrix<>::optImmediate,
    };

    // Old version before 3.3
    // K
    filename.clear() << folder << SEP << "dailyprofile." << study.inputExtension;
    ret = xcast.K.loadFromCSVFile(filename, 12, 24, mtrxOption, &tmpdata) && ret;

    // alpha
    filename.clear() << folder << SEP << "coeff_a." << study.inputExtension;
    ret = m.loadFromCSVFile(filename, 1, 12, mtrxOption, &tmpdata) && ret;
    xcast.data.pasteToColumn(XCast::dataCoeffAlpha, m[0]);

    // beta
    filename.clear() << folder << SEP << "coeff_b." << study.inputExtension;
    ret = m.loadFromCSVFile(filename, 1, 12, mtrxOption, &tmpdata) && ret;
    xcast.data.pasteToColumn(XCast::dataCoeffBeta, m[0]);

    // Delta
    filename.clear() << folder << SEP << "coeff_c." << study.inputExtension;
    ret = m.loadFromCSVFile(filename, 1, 12, mtrxOption, &tmpdata) && ret;
    xcast.data.pasteToColumn(XCast::dataCoeffDelta, m[0]);

    // Theta
    filename.clear() << folder << SEP << "coeff_tt." << study.inputExtension;
    ret = m.loadFromCSVFile(filename, 1, 12, mtrxOption, &tmpdata) && ret;
    xcast.data.pasteToColumn(XCast::dataCoeffTheta, m[0]);

    // Installed capacity
    IniFile ini;
    filename.clear() << folder << SEP << "settings.ini";
    if (ini.open(filename))
    {
        IniFile::Section* s;
        if (NULL != (s = ini.find("settings")))
        {
            xcast.capacity = s->read<double>("installedcapacity", 0.);
            if (xcast.capacity < 0.)
            {
                logs.error() << "Wind: Prepro: The installed capacity must be positive or null";
                xcast.capacity = 0.;
                ret = 0;
            }
        }
    }

    return ret;
}

bool Prepro::saveToFolder(const AnyString& folder) const
{
    return xcast.saveToFolder(folder);
}

} // namespace Wind
} // namespace Data
} // namespace Antares
