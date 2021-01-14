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
#include "../simulation/sim_structure_probleme_economique.h"
#include "../simulation/sim_structure_probleme_adequation.h"
#include "../simulation/sim_extern_variables_globales.h"
#include "../aleatoire/alea_fonctions.h"
#include <antares/timeelapsed.h>
#include <antares/emergency.h>
#include <antares/logs.h>
#include <antares/study.h>
#include <antares/memory/memory.h>
#include "../misc/cholesky.h"
#include "../misc/matrix-dp-make.h"

using namespace Antares;
using namespace Yuni;

#define SEP IO::Separator

#define EPSILON ((double)1.0e-9)

namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
static void PreproHydroInitMatrices(Data::Study& study, uint tsCount)
{
    study.areas.each([&](Data::Area& area) {
        auto& hydroseries = *(area.hydro.series);

        hydroseries.ror.resize(tsCount, HOURS_PER_YEAR);
        hydroseries.storage.resize(tsCount, DAYS_PER_YEAR);
        hydroseries.count = tsCount;

        hydroseries.flush();
    });
}

static void PreproRoundAllEntriesPlusDerated(Data::Study& study)
{
    bool derated = study.parameters.derated;

    study.areas.each([&](Data::Area& area) {
        auto& hydroseries = *(area.hydro.series);

        hydroseries.ror.roundAllEntries();
        hydroseries.storage.roundAllEntries();

        if (derated)
        {
            hydroseries.ror.averageTimeseries();
            hydroseries.storage.averageTimeseries();
        }

        hydroseries.flush();
    });
}

bool GenerateHydroTimeSeries(Data::Study& study, uint currentYear)
{
    logs.info() << "Generating the hydro time-series";

    Progression::Task progression(study, currentYear, Solver::Progression::sectTSGHydro);

    auto& studyRTI = *(study.runtime);
    auto& calendar = study.calendar;

    uint DIM = 12 * study.areas.size();
    uint DEM = DIM / 2;

    Matrix<double> CHSKY;
    CHSKY.reset(DIM, DIM);

    double* QCHOLTemp = new double[DIM];

    Matrix<double> B;
    B.reset(DIM, DIM);
    double x, y, z, u;
    double** nullmatrx = nullptr;

    if (1. > MatrixDPMake<double>(CHSKY.entry,
                                  study.preproHydroCorrelation.annual->entry,
                                  B.entry,
                                  nullmatrx,
                                  study.areas.size(),
                                  QCHOLTemp,
                                  true))
    {
        logs.error() << "TS Generator: Hydro: Invalid correlation matrix";
        AntaresSolverEmergencyShutdown();
    }

    CHSKY.flush();
    B.flush();
    Matrix<double> CORRE;
    CORRE.reset(DIM, DIM);

    for (uint i = 0; i < DIM; i++)
    {
        uint areaIndexI = i / 12;
        auto* prepro = study.areas.byIndex[areaIndexI]->hydro.prepro;

        auto& corre = CORRE[i];

        auto& annualCorrAreaI = (*study.preproHydroCorrelation.annual)[areaIndexI];

        for (uint j = 0; j < DIM; j++)
        {
            uint areaIndexJ = j / 12;
            auto* preproJ = study.areas.byIndex[areaIndexJ]->hydro.prepro;

            x = Math::Abs(((int)(i % 12) - (int)(j % 12)) / 2.);

            corre[j] = annualCorrAreaI[areaIndexJ]
                       * pow(prepro->intermonthlyCorrelation * preproJ->intermonthlyCorrelation, x);

            assert(not Math::NaN(corre[j]) and "TS generator Hydro: NaN value detected");
        }
    }

    {
        double r = MatrixDPMake<double>(
          CHSKY.entry, CORRE.entry, B.entry, nullmatrx, DIM, QCHOLTemp, true);
        if (r < 1.)
        {
            logs.warning() << " TS Generator: Hydro correlation matrix was shrinked by " << r;
            if (r < 0.)
                AntaresSolverEmergencyShutdown();
        }
    }

    Cholesky<double>(CHSKY.entry, B.entry, DIM, QCHOLTemp);

    B.clear();
    CORRE.clear();

    delete[] QCHOLTemp;
    QCHOLTemp = nullptr;

    double* NORM = new double[DIM];
    for (uint i = 0; i != DIM; ++i)
        NORM[i] = 0.;

    uint nbTimeseries = studyRTI.parameters->nbTimeSeriesHydro;

    PreproHydroInitMatrices(study, nbTimeseries);

    long cumul = 0;

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
            auto& area = *(study.areas.byIndex[i / 12]);
            auto& prepro = *area.hydro.prepro;
            auto& series = *area.hydro.series;
            auto& ror = series.ror[l];

            auto& colExpectation = prepro.data[Data::PreproHydro::expectation];
            auto& colStdDeviation = prepro.data[Data::PreproHydro::stdDeviation];
            auto& colMinEnergy = prepro.data[Data::PreproHydro::minimumEnergy];
            auto& colMaxEnergy = prepro.data[Data::PreproHydro::maximumEnergy];
            auto& colPOW = prepro.data[Data::PreproHydro::powerOverWater];

            uint month = i % 12;
            uint realmonth = calendar.months[month].realmonth;
            uint daysPerMonth = calendar.months[month].days;

            assert(l < series.ror.width);
            assert(not Math::NaN(colPOW[realmonth]));

            if (month == 0)
                cumul = 0;

            double EnergieHydrauliqueTotaleMensuelle = 0;

            if (not Math::Zero(colExpectation[realmonth]))
            {
                for (uint j = 0; j < i + 1; ++j)
                    EnergieHydrauliqueTotaleMensuelle += CHSKY[i][j] * NORM[j];

                EnergieHydrauliqueTotaleMensuelle *= colStdDeviation[realmonth];
                EnergieHydrauliqueTotaleMensuelle += colExpectation[realmonth];

                EnergieHydrauliqueTotaleMensuelle = exp(EnergieHydrauliqueTotaleMensuelle);
                assert(not Math::NaN(EnergieHydrauliqueTotaleMensuelle));

                if (EnergieHydrauliqueTotaleMensuelle < colMinEnergy[realmonth])
                    EnergieHydrauliqueTotaleMensuelle = colMinEnergy[realmonth];
                if (EnergieHydrauliqueTotaleMensuelle > colMaxEnergy[realmonth])
                    EnergieHydrauliqueTotaleMensuelle = colMaxEnergy[realmonth];
            }

            uint h = calendar.months[month].hours.first;
            uint end = calendar.months[month].hours.end;
            uint d = calendar.months[month].daysYear.first;
            uint dend = calendar.months[month].daysYear.end;

            double SIP = 0.;
            for (uint i = d; i < dend; i++)
                SIP += area.hydro.inflowPattern[0][i];

            if (Math::Zero(SIP))
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

            assert(not Math::NaN(monthlyStorage)
                   && "TS generator Hydro: NaN value detected in timeseries");

            if (Antares::Memory::swapSupport)
            {
                prepro.data.flush();
                series.flush();
            }

            cumul += daysPerMonth;
        }

        ++progression;
    }

    PreproRoundAllEntriesPlusDerated(study);

    if (0 != (study.parameters.timeSeriesToArchive & Data::timeSeriesHydro))
    {
        if (study.parameters.noOutput)
        {
            for (uint i = 0; i != study.areas.size(); ++i)
                ++progression;
        }
        else
        {
            logs.info() << "Archiving the hydro time-series";
            String output;
            study.areas.each([&](const Data::Area& area) {
                study.buffer.clear() << study.folderOutput << SEP << "ts-generator" << SEP
                                     << "hydro" << SEP << "mc-" << currentYear << SEP << area.id;
                if (IO::Directory::Create(study.buffer))
                {
                    output.clear() << study.buffer << SEP << "ror.txt";
                    area.hydro.series->ror.saveToCSVFile(output);

                    output.clear() << study.buffer << SEP << "storage.txt";
                    area.hydro.series->storage.saveToCSVFile(output);

                    area.hydro.series->flush();
                }

                ++progression;
            });
        }
    }

    delete[] NORM;

    if (Antares::Memory::swapSupport)
        study.preproHydroCorrelation.annual->flush();

    return true;
}

} // namespace TSGenerator
} // namespace Solver
} // namespace Antares
