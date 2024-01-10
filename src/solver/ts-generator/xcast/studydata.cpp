/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include "studydata.h"

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
{
StudyData::StudyData() : mode(Data::Correlation::modeNone)
{
    for (uint realmonth = 0; realmonth != 12; ++realmonth)
        correlation[realmonth] = nullptr;
}

StudyData::~StudyData()
{
    switch (mode)
    {
    case Data::Correlation::modeMonthly:
    {
        for (uint realmonth = 0; realmonth != 12; ++realmonth)
            delete correlation[realmonth];
        break;
    }
    case Data::Correlation::modeAnnual:
    {
        delete correlation[0];
        break;
    }
    case Data::Correlation::modeNone:
        break;
    }
}

void StudyData::prepareMatrix(Matrix<float>& m, const Matrix<float>& source) const
{
    uint areaCount = (uint)localareas.size();
    m.resize(areaCount, areaCount);
    m.fillUnit();

    for (uint x = 1; x < m.width; ++x)
    {
        uint areaXindx = localareas[x]->index;
        auto& sourceX = source[areaXindx];
        auto& mX = m[x];

        for (uint y = 0; y < x; ++y)
        {
            uint areaYindx = localareas[y]->index;
            float d = sourceX[areaYindx];
            mX[y] = d;
            m[y][x] = d;
        }
    }
}

void StudyData::reloadDataFromAreaList(const Data::Correlation& originalCorrelation)
{
    mode = originalCorrelation.mode();

    if (!localareas.empty())
    {
        switch (mode)
        {
        case Data::Correlation::modeAnnual:
        {
            auto* m = new Matrix<float>();
            prepareMatrix(*m, *(originalCorrelation.annual));

            for (uint realmonth = 0; realmonth != 12; ++realmonth)
                correlation[realmonth] = m;
            break;
        }
        case Data::Correlation::modeMonthly:
        {
            for (uint realmonth = 0; realmonth != 12; ++realmonth)
            {
                auto* m = new Matrix<float>();
                correlation[realmonth] = m;
                prepareMatrix(*m, originalCorrelation.monthly[realmonth]);
            }
            break;
        }
        case Data::Correlation::modeNone:
            break;
        }
    }
    else
    {
        for (uint realmonth = 0; realmonth != 12; ++realmonth)
            correlation[realmonth] = nullptr;
    }
}

} // namespace XCast
} // namespace TSGenerator
} // namespace Solver
} // namespace Antares
