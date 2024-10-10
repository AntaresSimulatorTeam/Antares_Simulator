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

#include "antares/solver/ts-generator/xcast/xcast.h"

#include <limits>
#include <sstream>
#include <string>

#include <yuni/yuni.h>

#include <antares/antares/fatal-error.h>
#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include "antares/solver/ts-generator/xcast/predicate.hxx"

namespace fs = std::filesystem;

namespace Antares::TSGenerator::XCast
{

enum
{
    alpha = Data::XCast::dataCoeffAlpha,
    beta = Data::XCast::dataCoeffBeta,
    gamma = Data::XCast::dataCoeffGamma,
    delta = Data::XCast::dataCoeffDelta,
    theta = Data::XCast::dataCoeffTheta,
    mu = Data::XCast::dataCoeffMu,
};

XCast::XCast(Data::Study& study, Data::TimeSeriesType ts, IResultWriter& writer):
    study(study),
    timeSeriesType(ts),
    pWriter(writer)
{
}

template<class PredicateT>
void XCast::exportTimeSeriesToTheOutput(Progression::Task& progression, PredicateT& predicate)
{
    if (study.parameters.noOutput)
    {
        for (uint i = 0; i != study.areas.size(); ++i)
        {
            ++progression;
        }
    }
    else
    {
        logs.info() << "Exporting " << predicate.timeSeriesName()
                    << " time-series into the output (year:" << year << ')';

        fs::path output = "ts-generator";
        output /= fs::path(predicate.timeSeriesName()) / std::string("mc-" + year);

        study.areas.each(
          [this, &progression, &predicate, &output](Data::Area& area)
          {
              std::string areaId = area.id + "txt";
              fs::path filename = output / areaId;
              std::string buffer;
              predicate.matrix(area).saveToBuffer(buffer);

              pWriter.addEntryFromBuffer(filename.string(), buffer);

              ++progression;
          });
    }
}

template<class PredicateT>
void XCast::applyTransferFunction(PredicateT& predicate)
{
    enum
    {
        x = 0,
        y = 1,
        maxPoints = Data::XCast::conversionMaxPoints,
    };

    uint h, i, j, k;

    float a[maxPoints];
    float b[maxPoints];

    const uint processCount = (uint)pData.localareas.size();

    for (uint s = 0; s != processCount; ++s)
    {
        if (pUseConversion[s])
        {
            auto& data = predicate.xcastData(*(pData.localareas[s]));

            auto& tf = data.conversion;

            uint last_i = 0;

            a[tf.width] = 0.f;
            b[tf.width] = 0.f;
            for (i = 0; i != tf.width - 1; ++i)
            {
                auto& p0 = tf[i];
                auto& p1 = tf[i + 1];
                if (p1[x] <= p0[x])
                {
                    std::ostringstream msg;
                    msg << "Transfer function: invalid X-coordinate at index (" << i << ", "
                        << (i + 1) << ")";
                    throw FatalError(msg.str());
                }
                a[i] = (p1[y] - p0[y]) / (p1[x] - p0[x]);
                b[i] = (p0[y] * p1[x] - p1[y] * p0[x]) / (p1[x] - p0[x]);
            }

            auto& dailyResults = DATA[s];
            for (h = 0; h != HOURS_PER_DAY; ++h)
            {
                for (i = 0; i != tf.width; ++i)
                {
                    j = (i + last_i) % (tf.width);
                    k = (i + last_i + 1) % (tf.width);
                    auto& pj = tf.column(j);
                    auto& pk = tf.column(k);

                    if (dailyResults[h] >= pj[x] && dailyResults[h] <= pk[x])
                    {
                        assert(!std::isinf(b[j]) && "Infinite value");
                        dailyResults[h] = (a[j] * dailyResults[h]) + b[j];
                        last_i = i;
                        break;
                    }
                }
            }
        }
    }
}

template<class PredicateT>
void XCast::updateMissingCoefficients(PredicateT& predicate)
{
    for (uint realmonth = 0; realmonth != 12; ++realmonth)
    {
        for (uint s = 0; s != pData.localareas.size(); ++s)
        {
            auto& data = predicate.xcastData(*(pData.localareas[s]));

            switch (data.distribution)
            {
            case Data::XCast::dtNormal:
            {
                float** v = data.data.entry;
                v[gamma][realmonth] = v[alpha][realmonth] - 6.f * v[beta][realmonth];
                v[delta][realmonth] = v[alpha][realmonth] + 6.f * v[beta][realmonth];
                break;
            }
            case Data::XCast::dtWeibullShapeA:
            {
                float** v = data.data.entry;
                v[delta][realmonth] = (float)GammaEuler(1. + 1. / v[alpha][realmonth]);
                break;
            }
            default:

                break;
            }
        }
    }
}

void XCast::allocateTemporaryData()
{
    uint p = (uint)pData.localareas.size();

    A.resize(p);
    B.resize(p);
    G.resize(p);
    D.resize(p);
    M.resize(p);
    T.resize(p);
    BO.resize(p);
    MA.resize(p);
    MI.resize(p);
    L.resize(p);
    POSI.resize(p);
    MAXI.resize(p);
    MINI.resize(p);
    ESPE.resize(p);
    STDE.resize(p);
    DIFF.resize(p);
    TREN.resize(p);
    WIEN.resize(p + 1);
    BROW.resize(p);

    BASI.resize(p);
    ALPH.resize(p);
    BETA.resize(p);

    D_COPIE.resize(p);

    pUseConversion.resize(p);

    Presque_maxi.resize(p);
    Presque_mini.resize(p);
    pQCHOLTotal.resize(p);

    CORR.resize(p);
    Triangle_reference.resize(p);
    Triangle_courant.resize(p);
    FO.resize(p);
    LISS.resize(p);
    DATL.resize(p);
    DATA.resize(p);
    Carre_courant.resize(p);
    Carre_reference.resize(p);

    for (uint i = 0; i != p; ++i)
    {
        Triangle_reference[i].resize(p);
        Triangle_courant[i].resize(p);
        Carre_courant[i].resize(p);
        Carre_reference[i].resize(p);

        CORR[i].resize(p);
        FO[i].resize(24);
        LISS[i].resize(24);
        DATL[i].resize(24);
        DATA[i].resize(24);
    }
}

template<class PredicateT>
bool XCast::runWithPredicate(PredicateT& predicate, Progression::Task& progression)
{
    pTSName = predicate.timeSeriesName();

    {
        logs.info();
        logs.info() << "Generating the " << predicate.timeSeriesName() << " time-series";
    }
    for (uint s = 0; s != pData.localareas.size(); ++s)
    {
        if (not predicate.preproDataIsReader(*pData.localareas[s]))
        {
            logs.warning()
              << "The timeseries will not be regenerated. All data related to the ts-generator for "
              << "'" << predicate.timeSeriesName() << "' have been released.";
            return false;
        }
    }

    if (pNeverInitialized)
    {
        loadFromStudy(predicate.correlation(study), predicate);

        allocateTemporaryData();

        for (uint s = 0; s != pData.localareas.size(); ++s)
        {
            auto& area = *(pData.localareas[s]);

            auto& xcast = predicate.xcastData(area);

            pUseConversion[s] = (xcast.useConversion && xcast.conversion.width >= 3);
        }

        pAccuracyOnCorrelation = ((study.parameters.timeSeriesAccuracyOnCorrelation
                                   & timeSeriesType)
                                  != 0);
    }

    const uint processCount = (uint)pData.localareas.size();

    if (study.areas.size() > pData.localareas.size())
    {
        progression += (nbTimeseries_ * DAYS_PER_YEAR)
                       * ((uint)study.areas.size() - (uint)pData.localareas.size());
    }

    if (processCount == 0)
    {
        if (study.parameters.timeSeriesToArchive & timeSeriesType)
        {
            exportTimeSeriesToTheOutput(progression, predicate);
        }
        return true;
    }

    updateMissingCoefficients(predicate);

    pComputedPointCount = 0;
    pNDPMatrixCount = 0;
    pLevellingCount = 0;

    for (uint tsIndex = 0; tsIndex != nbTimeseries_; ++tsIndex)
    {
        uint hourInTheYear = 0;

        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = study.calendar.months[month].realmonth;

            pNewMonth = true;

            pCorrMonth = &pData.correlation[realmonth];

            for (uint s = 0; s != processCount; ++s)
            {
                assert(s < pData.localareas.size() && "Bound checking");
                auto& xcastdata = predicate.xcastData(*(pData.localareas[s]));
                A[s] = xcastdata.data[alpha][realmonth];
                B[s] = xcastdata.data[beta][realmonth];
                G[s] = xcastdata.data[gamma][realmonth];
                D[s] = xcastdata.data[delta][realmonth];
                M[s] = (int)xcastdata.data[mu][realmonth];
                T[s] = xcastdata.data[theta][realmonth];
                L[s] = xcastdata.distribution;

                switch (xcastdata.distribution)
                {
                case Data::XCast::dtUniform:
                {
                    BO[s] = true;
                    MI[s] = G[s];
                    MA[s] = D[s];
                    break;
                }
                case Data::XCast::dtBeta:
                {
                    BO[s] = true;
                    MI[s] = G[s];
                    MA[s] = D[s];
                    break;
                }
                case Data::XCast::dtNormal:
                {
                    BO[s] = false;
                    MI[s] = -std::numeric_limits<float>::max();
                    MA[s] = +std::numeric_limits<float>::max();
                    break;
                }
                default:
                {
                    BO[s] = false;
                    MI[s] = G[s];
                    MA[s] = +std::numeric_limits<float>::max();
                }
                }
                memcpy(FO[s].data(), xcastdata.K[realmonth], sizeof(float) * HOURS_PER_DAY);
            }

            uint nbDaysPerMonth = study.calendar.months[month].days;
            for (uint j = 0; j != nbDaysPerMonth; ++j)
            {
                if (not generateValuesForTheCurrentDay())
                {
                    throw FatalError("xcast: Failed to generate values.");
                }

#ifndef NDEBUG

                for (uint s = 0; s != processCount; ++s)
                {
                    auto& dailyResults = DATA[s];

                    for (uint h = 0; h != HOURS_PER_DAY; ++h)
                    {
                        assert(!std::isinf(dailyResults[h]) && "Infinite value");
                    }
                }
#endif

                for (uint s = 0; s != processCount; ++s)
                {
                    assert(s < pData.localareas.size() && "Bound checking");
                    auto& currentArea = *pData.localareas[s];

                    auto& srcData = predicate.xcastData(currentArea);
                    if (srcData.useTranslation != Data::XCast::tsTranslationBeforeConversion)
                    {
                        continue;
                    }

                    auto& column = srcData.translation[0];
                    auto& dailyResults = DATA[s];
                    assert(hourInTheYear + HOURS_PER_DAY <= srcData.translation.height
                           && "Bound checking");

                    for (uint h = 0; h != HOURS_PER_DAY; ++h)
                    {
                        assert(!std::isinf(dailyResults[h]) && "Infinite value");
                        dailyResults[h] += (float)column[hourInTheYear + h];
                    }
                }

                applyTransferFunction(predicate);

#ifndef NDEBUG

                for (uint s = 0; s != processCount; ++s)
                {
                    auto& dailyResults = DATA[s];

                    for (uint h = 0; h != HOURS_PER_DAY; ++h)
                    {
                        assert(!std::isinf(dailyResults[h]) && "Infinite value");
                    }
                }
#endif

                for (uint s = 0; s != processCount; ++s)
                {
                    assert(s < pData.localareas.size() && "Bound checking");
                    auto& currentArea = *pData.localareas[s];

                    auto& srcData = predicate.xcastData(currentArea);

                    auto& series = predicate.matrix(currentArea);
                    assert(tsIndex < series.width);
                    auto& column = series.column(tsIndex);
                    auto& dailyResults = DATA[s];

                    for (uint h = 0; h != HOURS_PER_DAY; ++h)
                    {
                        assert(!std::isinf(dailyResults[h]) && "Infinite value");
                        dailyResults[h] *= (float)srcData.capacity;
                    }

                    if (srcData.useTranslation == Data::XCast::tsTranslationAfterConversion)
                    {
                        assert(hourInTheYear + HOURS_PER_DAY <= srcData.translation.height
                               && "Bound checking");
                        auto& tsavg = srcData.translation[0];
                        for (uint h = 0; h != HOURS_PER_DAY; ++h)
                        {
                            dailyResults[h] += (float)tsavg[hourInTheYear + h];
                        }
                    }

                    assert(hourInTheYear + HOURS_PER_DAY <= series.height && "Bound checking");
                    for (uint h = 0; h != HOURS_PER_DAY; ++h)
                    {
                        column[hourInTheYear + h] = std::round(dailyResults[h]);
                    }

                    ++progression;
                }

                hourInTheYear += HOURS_PER_DAY;

                pNewMonth = false;
            }
        }
    }

    {
        uint y = ((pAccuracyOnCorrelation) ? pComputedPointCount : (nbTimeseries_ * 365));
        uint z = pNDPMatrixCount;

        logs.info() << "  " << pComputedPointCount << " points calculated, using " << y
                    << " correlation matrices, out of which " << z << " were npsd and transformed";
    }

    if (study.parameters.derated)
    {
        study.areas.each([&predicate](Data::Area& area)
                         { predicate.matrix(area).averageTimeseries(); });
    }

    if (study.parameters.timeSeriesToArchive & timeSeriesType)
    {
        exportTimeSeriesToTheOutput(progression, predicate);
    }

    if (timeSeriesType == Data::timeSeriesLoad)
    {
        logs.debug() << "  [ts-generator] adding DSM";
        for (uint s = 0; s != study.areas.size(); ++s)
        {
            auto& area = *(study.areas.byIndex[s]);

            assert(static_cast<uint>(Data::fhrDSM) < area.reserves.width);

            auto& matrix = area.load.series.timeSeries;
            auto& dsmvalues = area.reserves.column(Data::fhrDSM);

            assert(matrix.width > 0);
            assert(matrix.height > 0);
            for (uint timeSeries = 0; timeSeries < matrix.width; ++timeSeries)
            {
                auto& perHour = matrix.column(timeSeries);
                for (uint h = 0; h < matrix.height; ++h)
                {
                    perHour[h] += dsmvalues[h];
                    assert(!std::isnan(perHour[h]));
                }
            }
        }
    }

    return true;
}

bool XCast::run()
{
    switch (timeSeriesType)
    {
    case Data::timeSeriesLoad:
    {
        Solver::Progression::Task p(study, year, Progression::sectTSGLoad);

        nbTimeseries_ = study.parameters.nbTimeSeriesLoad;

        Predicate::Load predicate;
        return runWithPredicate(predicate, p);
    }
    case Data::timeSeriesSolar:
    {
        Solver::Progression::Task p(study, year, Progression::sectTSGSolar);

        nbTimeseries_ = study.parameters.nbTimeSeriesSolar;

        Predicate::Solar predicate;
        return runWithPredicate(predicate, p);
    }
    case Data::timeSeriesWind:
    {
        Solver::Progression::Task p(study, year, Progression::sectTSGWind);

        nbTimeseries_ = study.parameters.nbTimeSeriesWind;

        Predicate::Wind predicate;
        return runWithPredicate(predicate, p);
    }

    default:
    {
        throw FatalError("xcast: Invalid time series type.");
    }
    }
    return false;
}

} // namespace Antares::TSGenerator::XCast
