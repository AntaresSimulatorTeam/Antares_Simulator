// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/ts-generator/xcast/studydata.h"

#include <yuni/yuni.h>

#include <antares/study/study.h>

using namespace Yuni;

namespace Antares::TSGenerator::XCast
{
StudyData::StudyData():
    mode(Data::Correlation::modeNone)
{
}

void StudyData::prepareMatrix(std::vector<std::vector<float>>& m, const Matrix& source) const
{
    uint areaCount = (uint)localareas.size();
    m.assign(areaCount, std::vector<float>(areaCount, 0.f));
    for (uint i = 0; i < areaCount; ++i)
    {
        m[i][i] = 1.f;
    }

    for (uint x = 1; x < m.size(); ++x)
    {
        uint areaXindx = localareas[x]->index;
        auto& sourceX = source[areaXindx];
        auto& mX = m[x];

        for (uint y = 0; y < x; ++y)
        {
            uint areaYindx = localareas[y]->index;
            const float d = static_cast<float>(sourceX[areaYindx]);
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
            for (uint realmonth = 0; realmonth != 12; ++realmonth)
            {
                prepareMatrix(correlation[realmonth], originalCorrelation.annual);
            }
            break;
        }
        case Data::Correlation::modeMonthly:
        {
            for (uint realmonth = 0; realmonth != 12; ++realmonth)
            {
                prepareMatrix(correlation[realmonth], originalCorrelation.monthly[realmonth]);
            }
            break;
        }
        case Data::Correlation::modeNone:
            break;
        }
    }
}

} // namespace Antares::TSGenerator::XCast
