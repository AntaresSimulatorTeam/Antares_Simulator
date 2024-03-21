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

#include <sstream>
#include <string>
#include <cmath>

#include <antares/study/study.h>
#include <antares/logs/logs.h>
#include <antares/writer/i_writer.h>

#include <antares/solver/ts-generator/generator.h>

#include "antares/study/simulation.h"

#define SEP Yuni::IO::Separator

#define FAILURE_RATE_EQ_1 0.999

namespace Antares::TSGenerator
{

ThermalInterface::ThermalInterface(Data::ThermalCluster *source) :
    unitCount(source->unitCount),
    nominalCapacity(source->nominalCapacity),
    forcedVolatility(source->forcedVolatility),
    plannedVolatility(source->plannedVolatility),
    forcedLaw(source->forcedLaw),
    plannedLaw(source->plannedLaw),
    prepro(source->prepro),
    series(source->series),
    modulationCapacity(source->modulation[Data::thermalModulationCapacity]),
    name(source->name())
{}

ThermalInterface::ThermalInterface(Data::AreaLink::LinkTsGeneration& source,
                                   Data::TimeSeries& capacity,
                                   const std::string& linkName) :
    unitCount(source.unitCount),
    nominalCapacity(source.nominalCapacity),
    forcedVolatility(source.forcedVolatility),
    plannedVolatility(source.plannedVolatility),
    forcedLaw(source.forcedLaw),
    plannedLaw(source.plannedLaw),
    prepro(source.prepro),
    series(capacity),
    modulationCapacity(source.modulationCapacity[0]),
    name(linkName)
{}

namespace
{

class GeneratorTempData final
{
public:
    explicit GeneratorTempData(Data::Study& study);

    void operator()(const Data::Area& area, ThermalInterface& cluster);

public:
    Data::Study& study;

    bool derated;

private:
    int durationGenerator(Data::ThermalLaw law, int expec, double volat, double a, double b);

    template<class T>
    void prepareIndispoFromLaw(Data::ThermalLaw law,
                               double volatility,
                               double A[],
                               double B[],
                               const T& duration);

private:
    uint nbThermalTimeseries_;

    MersenneTwister& rndgenerator;

    double AVP[366];
    enum
    {
        Log_size = 4000
    };
    int LOG[Log_size];
    int LOGP[Log_size];

    double lf[366];
    double lp[366];
    double ff[366];
    double pp[366];
    double af[366];
    double ap[366];
    double bf[366];
    double bp[366];

    std::vector<std::vector<double>> FPOW;
    std::vector<std::vector<double>> PPOW;

    Yuni::String pTempFilename;
};

GeneratorTempData::GeneratorTempData(Data::Study& study) :
    study(study),
    rndgenerator(study.runtime->random[Data::seedTsGenThermal])
{
    auto& parameters = study.parameters;

    nbThermalTimeseries_ = parameters.nbTimeSeriesThermal;

    derated = parameters.derated;

    FPOW.resize(DAYS_PER_YEAR);
    PPOW.resize(DAYS_PER_YEAR);
}

template<class T>
void GeneratorTempData::prepareIndispoFromLaw(Data::ThermalLaw law,
                                              double volatility,
                                              double A[],
                                              double B[],
                                              const T& duration)
{
    switch (law)
    {
    case Data::thermalLawUniform:
    {
        for (uint d = 0; d < DAYS_PER_YEAR; ++d)
        {
            double D = (double)duration[d];
            double xtemp = volatility * (D - 1.);
            A[d] = D - xtemp;
            B[d] = 2. * xtemp + 1.;
        }
        break;
    }
    case Data::thermalLawGeometric:
    {
        for (uint d = 0; d < DAYS_PER_YEAR; ++d)
        {
            double D = (double)duration[d];
            double xtemp = volatility * volatility * D * (D - 1.);
            if (xtemp != 0)
            {
                B[d] = 4. * xtemp + 1.;
                B[d] = sqrt(B[d]) - 1.;
                B[d] /= 2. * xtemp;
                A[d] = D - 1. / B[d];
                B[d] = log(1. - B[d]);
                B[d] = 1. / B[d];
            }
            else
            {
                B[d] = 1.;
                A[d] = 1.;
            }
        }
        break;
    }
    }
}

int GeneratorTempData::durationGenerator(Data::ThermalLaw law,
                                         int expec,
                                         double volat,
                                         double a,
                                         double b)
{
    if (volat == 0 or expec == 1)
        return expec;

    double rndnumber = rndgenerator.next();
    switch (law)
    {
    case Data::thermalLawUniform:
    {
        return (int(a + rndnumber * b));
    }
    case Data::thermalLawGeometric:
    {
        int resultat = (1 + int(a + (b)*log(rndnumber)));
        enum
        {
            limit = Log_size / 2 - 1
        };
        assert(limit == 1999);
        return (resultat <= limit) ? resultat : limit;
    }
    }
    assert(false and "return is missing");
    return 0;
}

void GeneratorTempData::operator()(const Data::Area& area, ThermalInterface& cluster)
{
    if (not cluster.prepro)
    {
        logs.error()
          << "Cluster: " << area.name << '/' << cluster.name
          << ": The timeseries will not be regenerated. All data related to the ts-generator for "
          << "'thermal' have been released.";
        return;
    }

    assert(cluster.prepro);

    if (0 == cluster.unitCount or 0 == cluster.nominalCapacity)
        return;

    const auto& preproData = *(cluster.prepro);

    int AUN = (int)cluster.unitCount;

    auto& FOD = preproData.data[Data::PreproThermal::foDuration];

    auto& POD = preproData.data[Data::PreproThermal::poDuration];

    auto& FOR = preproData.data[Data::PreproThermal::foRate];

    auto& POR = preproData.data[Data::PreproThermal::poRate];

    auto& NPOmin = preproData.data[Data::PreproThermal::npoMin];

    auto& NPOmax = preproData.data[Data::PreproThermal::npoMax];

    double f_volatility = cluster.forcedVolatility;

    double p_volatility = cluster.plannedVolatility;

    auto f_law = cluster.forcedLaw;

    auto p_law = cluster.plannedLaw;

    int FODOfTheDay;
    int PODOfTheDay;

    int FOD_reel = 0;
    int POD_reel = 0;

    for (uint d = 0; d < DAYS_PER_YEAR; ++d)
    {
        FPOW[d].resize(cluster.unitCount + 1);
        PPOW[d].resize(cluster.unitCount + 1);

        PODOfTheDay = (int)POD[d];
        FODOfTheDay = (int)FOD[d];

        lf[d] = FOR[d] / (FOR[d] + (FODOfTheDay) * (1. - FOR[d]));
        lp[d] = POR[d] / (POR[d] + (PODOfTheDay) * (1. - POR[d]));

        if (0. < lf[d] and lf[d] < lp[d])
            lf[d] *= (1. - lp[d]) / (1. - lf[d]);

        if (0. < lp[d] and lp[d] < lf[d])
            lp[d] *= (1. - lf[d]) / (1. - lp[d]);

        double a = 0.;
        double b = 0.;

        if (lf[d] <= FAILURE_RATE_EQ_1)
        {
            a = 1. - lf[d];
            ff[d] = lf[d] / a;
        }

        if (lp[d] <= FAILURE_RATE_EQ_1)
        {
            b = 1. - lp[d];
            pp[d] = lp[d] / b;
        }

        for (uint k = 0; k != cluster.unitCount + 1; ++k)
        {
            FPOW[d][k] = pow(a, (double)k);
            PPOW[d][k] = pow(b, (double)k);
        }
    }

    prepareIndispoFromLaw(f_law, f_volatility, af, bf, FOD);
    prepareIndispoFromLaw(p_law, p_volatility, ap, bp, POD);

    (void)::memset(AVP, 0, sizeof(AVP));
    (void)::memset(LOG, 0, sizeof(LOG));
    (void)::memset(LOGP, 0, sizeof(LOGP));

    int MXO = 0;

    int PPO = 0;

    int PFO = 0;

    int NOW = 0;

    int FUT = 0;

    int NPO_cur = 0;

    int stock = 0;

    double A = 0;
    double cumul = 0;
    double last = 0;

    double* dstSeries = nullptr;

    const uint tsCount = nbThermalTimeseries_ + 2;
    for (uint tsIndex = 0; tsIndex != tsCount; ++tsIndex)
    {
        uint hour = 0;

        if (tsIndex > 1)
            dstSeries = cluster.series.timeSeries[tsIndex - 2];

        for (uint dayInTheYear = 0; dayInTheYear < DAYS_PER_YEAR; ++dayInTheYear)
        {
            assert(dayInTheYear < 366);
            assert(not(lf[dayInTheYear] < 0.));
            assert(not(lp[dayInTheYear] < 0.));

            PODOfTheDay = (int)POD[dayInTheYear];
            FODOfTheDay = (int)FOD[dayInTheYear];

            assert(NOW < Log_size);
            NPO_cur -= LOGP[NOW];
            LOGP[NOW] = 0;
            AUN += LOG[NOW];
            LOG[NOW] = 0;

            if (NPO_cur > NPOmax[dayInTheYear])
            {
                int cible_retour = NPO_cur - (int)NPOmax[dayInTheYear];

                int cumul_retour = 0;

                for (int index = 1; index < Log_size; ++index)
                {
                    if (cumul_retour == cible_retour)
                        break;

                    if (LOGP[(NOW + index) % Log_size] + cumul_retour >= cible_retour)
                    {
                        LOGP[(NOW + index) % Log_size] -= (cible_retour - cumul_retour);

                        LOG[(NOW + index) % Log_size] -= (cible_retour - cumul_retour);

                        cumul_retour = cible_retour;
                    }
                    else
                    {
                        if (LOGP[(NOW + index) % Log_size] > 0)
                        {
                            cumul_retour += LOGP[(NOW + index) % Log_size];

                            LOG[(NOW + index) % Log_size] -= LOGP[(NOW + index) % Log_size];

                            LOGP[(NOW + index) % Log_size] = 0;
                        }
                    }
                }

                AUN += cible_retour;
                NPO_cur = (int)NPOmax[dayInTheYear];
            }

            int FOC = 0;

            int POC = 0;

            if (lf[dayInTheYear] > 0. and lf[dayInTheYear] <= FAILURE_RATE_EQ_1)
            {
                A = rndgenerator.next();
                last = FPOW[dayInTheYear][AUN];

                if (A > last)
                {
                    cumul = last;
                    for (int d = 1; d < AUN + 1; ++d)
                    {
                        last = ((last * ff[dayInTheYear]) * ((double)(AUN + 1. - d))) / (double)d;
                        cumul += last;
                        FOC = d;
                        if (A <= cumul)
                            break;
                    }
                }
            }
            else
            {
                FOC = (lf[dayInTheYear] > 0.) ? AUN : 0;
            }

            if (lp[dayInTheYear] > 0. and lp[dayInTheYear] <= FAILURE_RATE_EQ_1)
            {
                int AUN_app = AUN;
                if (stock >= 0 and stock <= AUN)
                    AUN_app -= stock;
                if (stock > AUN)
                    AUN_app = 0;

                last = PPOW[dayInTheYear][AUN_app];
                A = rndgenerator.next();

                if (A > last)
                {
                    cumul = last;
                    for (int d = 1; d < AUN_app + 1; ++d)
                    {
                        last
                          = ((last * pp[dayInTheYear]) * ((double)(AUN_app + 1. - d))) / (double)d;
                        cumul += last;
                        POC = d;
                        if (A <= cumul)
                            break;
                    }
                }
            }
            else
            {
                POC = (lp[dayInTheYear] > 0.) ? AUN : 0;
            }

            int candidat = POC + stock;
            if (0 <= candidat and candidat <= AUN)
            {
                POC = candidat;

                stock = 0;
            }
            if (candidat > AUN)
            {
                stock = candidat - AUN;

                POC = AUN;
            }
            if (candidat < 0)
            {
                stock = candidat;

                POC = 0;
            }

            if (POC + NPO_cur > NPOmax[dayInTheYear])
            {
                stock += POC + NPO_cur - (int)NPOmax[dayInTheYear];

                POC = (int)NPOmax[dayInTheYear] - NPO_cur;

                NPO_cur = (int)NPOmax[dayInTheYear];
            }
            else
            {
                if (POC + NPO_cur < NPOmin[dayInTheYear])
                {
                    if (NPOmin[dayInTheYear] - NPO_cur > AUN)
                    {
                        stock -= (AUN - POC);

                        POC = AUN;
                        NPO_cur += POC;
                    }
                    else
                    {
                        stock -= (int)NPOmin[dayInTheYear] - (POC + NPO_cur);

                        POC = (int)NPOmin[dayInTheYear] - NPO_cur;

                        NPO_cur = (int)NPOmin[dayInTheYear];
                    }
                }
                else
                {
                    NPO_cur += POC;
                }
            }

            if (cluster.unitCount == 1)
            {
                if (POC == 1 and FOC == 1)
                {
                    PPO = 0;
                    PFO = 0;
                    MXO = 1;
                }
                else
                {
                    MXO = 0;
                    PFO = FOC;
                    PPO = POC;
                }
            }
            else
            {
                if (AUN != 0)
                {
                    MXO = POC * FOC / AUN;
                    PPO = POC - MXO;
                    PFO = FOC - MXO;
                }
                else
                {
                    MXO = 0;
                    PPO = 0;
                    PFO = 0;
                }
            }

            AUN = AUN - (PPO + PFO + MXO);

            if (PFO != 0 or MXO != 0)
                FOD_reel = durationGenerator(
                  f_law, FODOfTheDay, f_volatility, af[dayInTheYear], bf[dayInTheYear]);
            if (PPO != 0 or MXO != 0)
                POD_reel = durationGenerator(
                  p_law, PODOfTheDay, p_volatility, ap[dayInTheYear], bp[dayInTheYear]);

            assert(FUT < Log_size);
            if (PFO != 0)
            {
                FUT = (NOW + FOD_reel) % Log_size;
                LOG[FUT] += PFO;
            }
            if (PPO != 0)
            {
                FUT = (NOW + POD_reel) % Log_size;
                LOG[FUT] += PPO;
                LOGP[FUT] = LOGP[FUT] + PPO;
            }
            if (MXO != 0)
            {
                FUT = (NOW + POD_reel + FOD_reel) % Log_size;
                LOG[FUT] += MXO;
                LOGP[FUT] = LOGP[FUT] + MXO;
            }
            NOW = (NOW + 1) % Log_size;

            AVP[dayInTheYear] = AUN * cluster.nominalCapacity;

            if (tsIndex > 1)
            {
                double AVPDayInTheYear = AVP[dayInTheYear];
                for (uint h = 0; h != 24; ++h)
                {
                    dstSeries[hour] = std::round(AVPDayInTheYear * cluster.modulationCapacity[hour]);
                    ++hour;
                }
            }
        }
    }

    if (derated)
        cluster.series.timeSeries.averageTimeseries();

}
} // namespace

void writeResultsToDisk(const Data::Study& study,
                        Solver::IResultWriter& writer,
                        const Data::Area& area,
                        const Data::ThermalCluster& cluster,
                        const std::string& savePath)
{
    if (study.parameters.noOutput)
        return;

    Yuni::String pTempFilename;
    pTempFilename.reserve(study.folderOutput.size() + 256);

    pTempFilename.clear() << savePath << SEP << area.id << SEP << cluster.id() << ".txt";

    enum
    {
        precision = 0
    };

    std::string buffer;
    cluster.series.timeSeries.saveToBuffer(buffer, precision);

    writer.addEntryFromBuffer(pTempFilename.c_str(), buffer);
}

std::vector<Data::ThermalCluster*> getAllClustersToGen(Data::AreaList& areas,
                                                       bool globalThermalTSgeneration)
{
    std::vector<Data::ThermalCluster*> clusters;

    areas.each([&clusters, &globalThermalTSgeneration](Data::Area& area) {
        for (auto cluster : area.thermal.list.all())
            if (cluster->doWeGenerateTS(globalThermalTSgeneration))
                clusters.push_back(cluster.get());
    });

    return clusters;
}

bool GenerateThermalTimeSeries(Data::Study& study,
                               std::vector<Data::ThermalCluster*> clusters,
                               Solver::IResultWriter& writer,
                               const std::string& savePath)
{
    logs.info();
    logs.info() << "Generating the thermal time-series";

    bool archive = (0 != (study.parameters.timeSeriesToArchive & Data::timeSeriesThermal));

    auto generator = std::make_unique<GeneratorTempData>(study);

    // TODO VP: parallel
    for (auto* cluster : clusters)
    {
        ThermalInterface clusterInterface(cluster);
        (*generator)(*cluster->parentArea, clusterInterface);

        if (archive)
            writeResultsToDisk(study, writer, *cluster->parentArea, *cluster, savePath);

        cluster->calculationOfSpinning();
    }

    return true;
}

} // namespace Antares::TSGenerator
