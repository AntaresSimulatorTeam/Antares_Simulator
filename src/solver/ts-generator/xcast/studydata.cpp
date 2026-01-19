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
