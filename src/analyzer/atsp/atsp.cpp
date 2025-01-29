/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "atsp.h"

using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace Antares
{
// constants
const uint ATSP::lonmois[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const uint ATSP::durmois[12] = {744, 672, 744, 720, 744, 720, 744, 744, 720, 744, 720, 744};
const uint ATSP::posmois[12] = {0, 744, 1416, 2160, 2880, 3624, 4344, 5088, 5832, 6552, 7296, 8016};

ATSP::ATSP():
    pRoundingCount(),
    pRounding80percentCount(),
    pRoundingCountTotal(),
    HOR(0.92),
    pLimitMemory(200 * 1024 * 1024),
    pCacheMemoryUsed(),
    pAutoClean(false)
{
}

ATSP::~ATSP()
{
    if (pAutoClean)
    {
        logs.info() << "Cleaning...";
        pStr.clear();
#ifdef YUNI_OS_WINDOWS
        pStr << "del /S /F /Q ";
        pStr << '"' << pTemp << SEP << tsName << '"';
        system(pStr.c_str());
        pStr.clear();
        pStr << "rmdir /S /Q ";
        pStr << '"' << pTemp << SEP << tsName << '"';
        system(pStr.c_str());
        pStr.clear();
        pStr << "rmdir /Q ";
        pStr << '"' << pTemp << '"';
        system(pStr.c_str());
#else
        pStr << "rm -rf ";
        pStr << '"' << pTemp << SEP << tsName << '"';
        system(pStr.c_str());
#endif
    }
}

void ATSP::printSummary() const
{
    logs.info();
    logs.info() << "  Summary :";
    logs.info() << "  number of timeseries : " << pTimeseriesCount;
    logs.info() << "  short-term autocorr adjustment   : " << pShortTermAutoCorrAdjustment;
    logs.info() << "  medium-term autocorr. adjustment : " << pMediumTermAutoCorrAdjustment;
    logs.info() << "  trimming threshold               : " << pRoundOff;

    if (pUseUpperBound)
    {
        logs.info() << "  upper bound  : " << pUpperBound;
    }
    else
    {
        logs.info() << "  upper bound  : (none)";
    }

    if (pUseLowerBound)
    {
        logs.info() << "  lower bound  : " << pLowerBound;
    }
    else
    {
        logs.info() << "  lower bound  : (none)";
    }

    logs.info() << "  memory cache size : " << (pLimitMemory / 1024 / 1024) << "Mo";
    logs.info() << "  auto-clean : " << (pAutoClean ? "yes" : "no");

    logs.info();
    if (pArea.size() > 1)
    {
        logs.info() << "  " << pArea.size() << " areas to analyze";
    }
    else
    {
        logs.info() << "  1 area to analyze";
    }

    for (uint i = 0; i != pArea.size(); ++i)
    {
        const AreaInfo& info = pArea[i];
        if (info.rawData)
        {
            logs.info() << "  " << info.name << ": law '"
                        << Data::XCast::DistributionToCString(info.distribution)
                        << "' will be fitted on raw data";
        }
        else
        {
            logs.info() << "  " << info.name << ": law '"
                        << Data::XCast::DistributionToCString(info.distribution)
                        << "' will be fitted on deviation from average data";
        }
    }
}

bool ATSP::writeMoments() const
{
    IO::File::Stream f;
    {
        String filename;
        filename.clear() << pTemp << SEP << tsName << SEP << "moments-table.txt";
        if (!f.openRW(filename))
        {
            logs.error() << "Impossible to create " << filename;
            return false;
        }
    }

    f << '\t';
    for (uint j = 0; j < 12; ++j)
    {
        for (uint k = 0; k < 4; ++k)
        {
            f << "MONTH " << (j + 1) << '\t';
        }
    }
    f << "\n\t";

    for (uint j = 0; j < 12; ++j)
    {
        f << " EXPEC\t STAND\t SKEWN\t KURTO\t";
    }
    f << '\n';

    for (uint i = 0; i < pArea.size(); ++i)
    {
        const AreaInfo& info = pArea[i];
        if (!info.enabled)
        {
            continue;
        }

        f << info.name << '\t';
        const MomentCentrSingle& moment = moments_centr_net[i];

        for (uint j = 0; j < 12; ++j)
        {
            const double* m = moment.data[j];

            for (uint k = 0; k < 4; ++k)
            {
                f << m[k] << '\t';
            }
        }
        f << '\n';
    }

    return true;
}

bool ATSP::cachePreload(unsigned index,
                        const AnyString& filename,
                        uint height,
                        Matrix<>::BufferType& buffer)
{
    enum
    {
        options = Matrix<>::optImmediate | Matrix<>::optFixedSize,
    };

    if (pCacheMatrix[index].loadFromCSVFile(filename, NBS, height, options, &buffer))
    {
        pCacheLastValidIndex = index + 1;
        return true;
    }
    else
    {
        pCacheMatrix[index].clear();
    }

    return false;
}

} // namespace Antares
