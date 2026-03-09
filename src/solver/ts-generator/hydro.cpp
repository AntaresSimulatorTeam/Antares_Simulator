// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cmath>
#include <fmt/format.h>

#include <antares/antares/fatal-error.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include <antares/writer/i_writer.h>
#include "antares/solver/misc/cholesky.h"
#include "antares/solver/misc/matrix-dp-make.h"

using namespace Antares;

namespace fs = std::filesystem;

#define EPSILON ((double)1.0e-9)

namespace Antares::TSGenerator
{

static void PreproRoundAllEntriesPlusDerated(Data::Study& study)
{
    bool derated = study.parameters.derated;

    study.areas.each(
      [&derated](Data::Area& area)
      {
          auto& hydroseries = *(area.hydro.series);

          hydroseries.ror.roundAllEntries();
          hydroseries.storage.roundAllEntries();

          if (derated)
          {
              hydroseries.ror.averageTimeseries();
              hydroseries.storage.averageTimeseries();
          }
      });
}

bool GenerateHydroTimeSeries(Data::Study& study, Solver::IResultWriter& writer)
{
    logs.info() << "Generating the hydro time-series";

    auto& studyRTI = study.runtime;
    auto& calendar = study.calendar;

    uint DIM = MONTHS_PER_YEAR * study.areas.size();
    uint DEM = DIM / 2;

    Matrix<double> CHSKY;
    CHSKY.reset(DIM, DIM);

    std::vector<double> QCHOLTemp(DIM);

    Matrix<double> B;
    B.reset(DIM, DIM);
    double x, y, z, u;
    double** nullmatrx = nullptr;

    if (1. > Solver::MatrixDPMake<double>(CHSKY.entry,
                                          study.preproHydroCorrelation.annual.entry,
                                          B.entry,
                                          nullmatrx,
                                          study.areas.size(),
                                          QCHOLTemp.data(),
                                          true))
    {
        throw FatalError("TS Generator: Hydro: Invalid correlation matrix");
    }

    Matrix<double> CORRE;
    CORRE.reset(DIM, DIM);

    for (uint i = 0; i < DIM; i++)
    {
        uint areaIndexI = i / MONTHS_PER_YEAR;
        auto* prepro = study.areas.byIndex[areaIndexI]->hydro.prepro.get();

        auto& corre = CORRE[i];

        auto& annualCorrAreaI = study.preproHydroCorrelation.annual[areaIndexI];

        for (uint j = 0; j < DIM; j++)
        {
            uint areaIndexJ = j / MONTHS_PER_YEAR;
            auto* preproJ = study.areas.byIndex[areaIndexJ]->hydro.prepro.get();

            x = std::abs(((int)(i % MONTHS_PER_YEAR) - (int)(j % MONTHS_PER_YEAR)) / 2.);

            corre[j] = annualCorrAreaI[areaIndexJ]
                       * pow(prepro->intermonthlyCorrelation * preproJ->intermonthlyCorrelation, x);

            assert(not std::isnan(corre[j]) and "TS generator Hydro: NaN value detected");
        }
    }

    {
        double r = Solver::MatrixDPMake<double>(CHSKY.entry,
                                                CORRE.entry,
                                                B.entry,
                                                nullmatrx,
                                                DIM,
                                                QCHOLTemp.data(),
                                                true);
        if (r < 1.)
        {
            logs.warning() << " TS Generator: Hydro correlation matrix was shrinked by " << r;
            if (r < 0.)
            {
                throw FatalError("TS Generator: r must be positive");
            }
        }
    }

    Solver::Cholesky<double>(CHSKY.entry, B.entry, DIM, QCHOLTemp.data());

    B.clear();
    CORRE.clear();
    QCHOLTemp.clear();

    std::vector<double> NORM(DIM);
    for (uint i = 0; i != DIM; ++i)
    {
        NORM[i] = 0.;
    }

    uint nbTimeseries = study.parameters.nbTimeSeriesHydro;

    for (uint l = 0; l != nbTimeseries; ++l)
    {
        for (uint s = 0; s < DEM; ++s)
        {
            do
            {
                x = 2. * studyRTI.random[Data::seedTsGenHydro].next() - 1.;
                y = 2. * studyRTI.random[Data::seedTsGenHydro].next() - 1.;
                z = (x * x) + (y * y);
            } while (z > 1);

            u = sqrt(-2. * log(z) / z);
            NORM[2 * s] = x * u;
            NORM[2 * s + 1] = y * u;
        }
        for (uint i = 0; i < DIM; ++i)
        {
            auto& area = *(study.areas.byIndex[i / MONTHS_PER_YEAR]);
            auto& prepro = *area.hydro.prepro;
            auto& series = *area.hydro.series;
            auto ror = series.ror[l];

            auto& colExpectation = prepro.data[Data::PreproHydro::expectation];
            auto& colStdDeviation = prepro.data[Data::PreproHydro::stdDeviation];
            auto& colMinEnergy = prepro.data[Data::PreproHydro::minimumEnergy];
            auto& colMaxEnergy = prepro.data[Data::PreproHydro::maximumEnergy];
            auto& colPOW = prepro.data[Data::PreproHydro::powerOverWater];

            uint month = i % MONTHS_PER_YEAR;
            uint realmonth = calendar.months[month].realmonth;

            assert(l < series.ror.timeSeries.width);
            assert(not std::isnan(colPOW[realmonth]));

            double EnergieHydrauliqueTotaleMensuelle = 0;

            if (!Utils::isZero(colExpectation[realmonth]))
            {
                for (uint j = 0; j < i + 1; ++j)
                {
                    EnergieHydrauliqueTotaleMensuelle += CHSKY[i][j] * NORM[j];
                }

                EnergieHydrauliqueTotaleMensuelle *= colStdDeviation[realmonth];
                EnergieHydrauliqueTotaleMensuelle += colExpectation[realmonth];

                EnergieHydrauliqueTotaleMensuelle = exp(EnergieHydrauliqueTotaleMensuelle);
                assert(not std::isnan(EnergieHydrauliqueTotaleMensuelle));

                if (EnergieHydrauliqueTotaleMensuelle < colMinEnergy[realmonth])
                {
                    EnergieHydrauliqueTotaleMensuelle = colMinEnergy[realmonth];
                }
                if (EnergieHydrauliqueTotaleMensuelle > colMaxEnergy[realmonth])
                {
                    EnergieHydrauliqueTotaleMensuelle = colMaxEnergy[realmonth];
                }
            }

            uint h = calendar.months[month].hours.first;
            uint end = calendar.months[month].hours.end;
            uint d = calendar.months[month].daysYear.first;
            uint dend = calendar.months[month].daysYear.end;

            double SIP = 0.;
            for (uint i = d; i < dend; i++)
            {
                SIP += area.hydro.inflowPattern[0][i];
            }

            if (Utils::isZero(SIP))
            {
                logs.fatal() << "Sum of monthly inflow patterns equals zero.";
                return false;
            }

            double dailyStorage = 0.;
            double hourlyStorage = 0.;
            double dailyInflowPattern = 0.;
            double sumInflowPatterns = SIP;

            double monthlyROR = EnergieHydrauliqueTotaleMensuelle * colPOW[realmonth];

            for (; h != end; ++h)
            {
                uint dayOfHour = h / 24;
                dailyInflowPattern = area.hydro.inflowPattern[0][dayOfHour];
                hourlyStorage = round(monthlyROR * dailyInflowPattern / (24. * sumInflowPatterns));

                ror[h] = hourlyStorage;

                monthlyROR -= hourlyStorage;
                sumInflowPatterns -= dailyInflowPattern / 24.;
            }

            double monthlyStorage = EnergieHydrauliqueTotaleMensuelle * (1. - colPOW[realmonth]);

            sumInflowPatterns = SIP;
            dailyStorage = 0.;
            dailyInflowPattern = 0.;
            for (; d != dend; ++d)
            {
                dailyInflowPattern = area.hydro.inflowPattern[0][d];
                dailyStorage = round(monthlyStorage * dailyInflowPattern / sumInflowPatterns);

                series.storage[l][d] = dailyStorage;

                monthlyStorage -= dailyStorage;
                sumInflowPatterns -= dailyInflowPattern;
            }

            assert(not std::isnan(monthlyStorage)
                   && "TS generator Hydro: NaN value detected in timeseries");
        }
    }

    PreproRoundAllEntriesPlusDerated(study);

    if (0 != (study.parameters.timeSeriesToArchive & Data::timeSeriesHydro))
    {
        if (!study.parameters.noOutput)
        {
            logs.info() << "Archiving the hydro time-series";
            study.areas.each(
              [&writer](const Data::Area& area)
              {
                  const int precision = 0;
                  const std::string mcYear = "mc-0";
                  fs::path outputFolder = fs::path("ts-generator") / "hydro" / mcYear
                                          / area.id.to<std::string>();

                  std::string buffer;
                  area.hydro.series->ror.timeSeries.saveToBuffer(buffer, precision);
                  fs::path outputFile = outputFolder / "ror.txt";
                  writer.addEntryFromBuffer(outputFile, buffer);

                  area.hydro.series->storage.timeSeries.saveToBuffer(buffer, precision);
                  outputFile = outputFolder / "storage.txt";
                  writer.addEntryFromBuffer(outputFile, buffer);
              });
        }
    }

    return true;
}

} // namespace Antares::TSGenerator
