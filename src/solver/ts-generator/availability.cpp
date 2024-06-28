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

#include <cmath>
#include <sstream>
#include <string>

#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/generator.h>
#include <antares/solver/ts-generator/law.h>
#include <antares/study/study.h>
#include <antares/writer/i_writer.h>
#include <antares/io/file.h> // For Antares::IO::fileSetContent
#include "antares/study/simulation.h"

#define SEP Yuni::IO::Separator
namespace fs = std::filesystem;

constexpr double FAILURE_RATE_EQ_1 = 0.999;

namespace Antares::TSGenerator
{
AvailabilityTSGeneratorData::AvailabilityTSGeneratorData(Data::ThermalCluster* cluster):
    unitCount(cluster->unitCount),
    nominalCapacity(cluster->nominalCapacity),
    forcedVolatility(cluster->forcedVolatility),
    plannedVolatility(cluster->plannedVolatility),
    forcedLaw(cluster->forcedLaw),
    plannedLaw(cluster->plannedLaw),
    prepro(cluster->prepro),
    series(cluster->series.timeSeries),
    modulationCapacity(cluster->modulation[Data::thermalModulationCapacity]),
    name(cluster->name())
{
}

AvailabilityTSGeneratorData::AvailabilityTSGeneratorData(LinkTSgenerationParams& source,
                                                         Data::TimeSeries& capacity,
                                                         Matrix<>& modulation,
                                                         const std::string& areaDestName):
    unitCount(source.unitCount),
    nominalCapacity(source.nominalCapacity),
    forcedVolatility(source.forcedVolatility),
    plannedVolatility(source.plannedVolatility),
    forcedLaw(source.forcedLaw),
    plannedLaw(source.plannedLaw),
    prepro(source.prepro.get()),
    series(capacity.timeSeries),
    modulationCapacity(modulation[0]),
    name(areaDestName)
{
}

namespace
{
class AvailabilityTSgenerator final
{
public:
    explicit AvailabilityTSgenerator(Data::Study&, unsigned, MersenneTwister&);
    explicit AvailabilityTSgenerator(bool, unsigned, MersenneTwister&);

    void run(AvailabilityTSGeneratorData&) const;

private:
    bool derated;

    uint nbOfSeriesToGen_;

    MersenneTwister& rndgenerator;

    static constexpr int Log_size = 4000;

    int durationGenerator(Data::StatisticalLaw law,
                          int expec,
                          double volat,
                          double a,
                          double b) const;

    template<class T>
    void prepareIndispoFromLaw(Data::StatisticalLaw law,
                               double volatility,
                               std::array<double, 366>& A,
                               std::array<double, 366>& B,
                               const T& duration) const;
};

AvailabilityTSgenerator::AvailabilityTSgenerator(Data::Study& study,
                                                 unsigned nbOfSeriesToGen,
                                                 MersenneTwister& rndGenerator):
    derated(study.parameters.derated),
    nbOfSeriesToGen_(nbOfSeriesToGen),
    rndgenerator(rndGenerator)
{
}

AvailabilityTSgenerator::AvailabilityTSgenerator(bool derated,
                                                 unsigned int nbOfSeriesToGen,
                                                 MersenneTwister& rndGenerator):
    derated(derated),
    nbOfSeriesToGen_(nbOfSeriesToGen),
    rndgenerator(rndGenerator)
{
}

template<class T>
void AvailabilityTSgenerator::prepareIndispoFromLaw(Data::StatisticalLaw law,
                                                    double volatility,
                                                    std::array<double, 366>& A,
                                                    std::array<double, 366>& B,
                                                    const T& duration) const
{
    switch (law)
    {
    case Data::LawUniform:
    {
        for (uint d = 0; d < DAYS_PER_YEAR; ++d)
        {
            double D = duration[d];
            double xtemp = volatility * (D - 1.);
            A[d] = D - xtemp;
            B[d] = 2. * xtemp + 1.;
        }
        break;
    }
    case Data::LawGeometric:
    {
        for (uint d = 0; d < DAYS_PER_YEAR; ++d)
        {
            double D = duration[d];
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

int AvailabilityTSgenerator::durationGenerator(Data::StatisticalLaw law,
                                               int expec,
                                               double volat,
                                               double a,
                                               double b) const
{
    if (volat == 0 || expec == 1)
    {
        return expec;
    }

    double rndnumber = rndgenerator.next();

    switch (law)
    {
    case Data::LawUniform:
    {
        return (int(a + rndnumber * b));
    }
    case Data::LawGeometric:
    {
        int resultat = (1 + int(a + (b)*log(rndnumber)));
        int limit = Log_size / 2 - 1;

        assert(limit == 1999);
        return (resultat <= limit) ? resultat : limit;
    }
    }
    assert(false && "return is missing");
    return 0;
}

void AvailabilityTSgenerator::run(AvailabilityTSGeneratorData& tsGenerationData) const
{
    assert(tsGenerationData.prepro);

    if (0 == tsGenerationData.unitCount || 0 == tsGenerationData.nominalCapacity)
    {
        return;
    }

    const auto& preproData = *(tsGenerationData.prepro);

    int AUN = tsGenerationData.unitCount;

    auto& FOD = preproData.data[Data::PreproAvailability::foDuration];

    auto& POD = preproData.data[Data::PreproAvailability::poDuration];

    auto& FOR = preproData.data[Data::PreproAvailability::foRate];

    auto& POR = preproData.data[Data::PreproAvailability::poRate];

    auto& NPOmin = preproData.data[Data::PreproAvailability::npoMin];

    auto& NPOmax = preproData.data[Data::PreproAvailability::npoMax];

    double f_volatility = tsGenerationData.forcedVolatility;

    double p_volatility = tsGenerationData.plannedVolatility;

    auto f_law = tsGenerationData.forcedLaw;

    auto p_law = tsGenerationData.plannedLaw;

    std::vector<std::vector<double>> FPOW(DAYS_PER_YEAR);
    std::vector<std::vector<double>> PPOW(DAYS_PER_YEAR);

    std::array<double, 366> lf;
    std::array<double, 366> lp;
    std::array<double, 366> ff;
    std::array<double, 366> pp;
    std::array<double, 366> af;
    std::array<double, 366> ap;
    std::array<double, 366> bf;
    std::array<double, 366> bp;

    int FODOfTheDay;
    int PODOfTheDay;

    int FOD_reel = 0;
    int POD_reel = 0;

    for (uint d = 0; d < DAYS_PER_YEAR; ++d)
    {
        FPOW[d].resize(tsGenerationData.unitCount + 1);
        PPOW[d].resize(tsGenerationData.unitCount + 1);

        PODOfTheDay = (int)POD[d];
        FODOfTheDay = (int)FOD[d];

        lf[d] = FOR[d] / (FOR[d] + (FODOfTheDay) * (1. - FOR[d]));
        lp[d] = POR[d] / (POR[d] + (PODOfTheDay) * (1. - POR[d]));

        if (0. < lf[d] && lf[d] < lp[d])
        {
            lf[d] *= (1. - lp[d]) / (1. - lf[d]);
        }

        if (0. < lp[d] && lp[d] < lf[d])
        {
            lp[d] *= (1. - lf[d]) / (1. - lp[d]);
        }

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

        for (uint k = 0; k != tsGenerationData.unitCount + 1; ++k)
        {
            FPOW[d][k] = pow(a, (double)k);
            PPOW[d][k] = pow(b, (double)k);
        }
    }

    prepareIndispoFromLaw(f_law, f_volatility, af, bf, FOD);
    prepareIndispoFromLaw(p_law, p_volatility, ap, bp, POD);

    std::array<double, 366> AVP{};
    std::array<double, Log_size> LOG{};
    std::array<double, Log_size> LOGP{};

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

    auto& modulation = tsGenerationData.modulationCapacity;
    double* dstSeries = nullptr;

    const uint tsCount = nbOfSeriesToGen_ + 2;
    for (uint tsIndex = 0; tsIndex != tsCount; ++tsIndex)
    {
        uint hour = 0;

        if (tsIndex > 1)
        {
            dstSeries = tsGenerationData.series[tsIndex - 2];
        }

        for (uint dayInTheYear = 0; dayInTheYear < DAYS_PER_YEAR; ++dayInTheYear)
        {
            assert(dayInTheYear < 366);
            assert(lf[dayInTheYear] >= 0.);
            assert(lp[dayInTheYear] >= 0.);

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
                    {
                        break;
                    }

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

            if (lf[dayInTheYear] > 0. && lf[dayInTheYear] <= FAILURE_RATE_EQ_1)
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
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                FOC = (lf[dayInTheYear] > 0.) ? AUN : 0;
            }

            if (lp[dayInTheYear] > 0. && lp[dayInTheYear] <= FAILURE_RATE_EQ_1)
            {
                int AUN_app = AUN;
                if (stock >= 0 && stock <= AUN)
                {
                    AUN_app -= stock;
                }
                if (stock > AUN)
                {
                    AUN_app = 0;
                }

                last = PPOW[dayInTheYear][AUN_app];
                A = rndgenerator.next();

                if (A > last)
                {
                    cumul = last;
                    for (int d = 1; d < AUN_app + 1; ++d)
                    {
                        last = ((last * pp[dayInTheYear]) * ((double)(AUN_app + 1. - d)))
                               / (double)d;
                        cumul += last;
                        POC = d;
                        if (A <= cumul)
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                POC = (lp[dayInTheYear] > 0.) ? AUN : 0;
            }

            int candidat = POC + stock;
            if (0 <= candidat && candidat <= AUN)
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

            if (tsGenerationData.unitCount == 1)
            {
                if (POC == 1 && FOC == 1)
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

            if (PFO != 0 || MXO != 0)
            {
                FOD_reel = durationGenerator(f_law,
                                             FODOfTheDay,
                                             f_volatility,
                                             af[dayInTheYear],
                                             bf[dayInTheYear]);
            }
            if (PPO != 0 || MXO != 0)
            {
                POD_reel = durationGenerator(p_law,
                                             PODOfTheDay,
                                             p_volatility,
                                             ap[dayInTheYear],
                                             bp[dayInTheYear]);
            }

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

            AVP[dayInTheYear] = AUN * tsGenerationData.nominalCapacity;

            if (tsIndex > 1)
            {
                double AVPDayInTheYear = AVP[dayInTheYear];
                for (uint h = 0; h != 24; ++h)
                {
                    dstSeries[hour] = std::round(AVPDayInTheYear * modulation[hour]);
                    ++hour;
                }
            }
        }
    }

    if (derated)
    {
        tsGenerationData.series.averageTimeseries();
    }
}
} // namespace

std::vector<Data::ThermalCluster*> getAllClustersToGen(const Data::AreaList& areas,
                                                       bool globalThermalTSgeneration)
{
    std::vector<Data::ThermalCluster*> clusters;

    areas.each(
      [&clusters, &globalThermalTSgeneration](const Data::Area& area)
      {
          for (const auto& cluster: area.thermal.list.all())
          {
              if (cluster->doWeGenerateTS(globalThermalTSgeneration))
              {
                  clusters.push_back(cluster.get());
              }
          }
      });

    return clusters;
}

void writeTStoDisk(const Matrix<>& series,
                   const std::filesystem::path savePath)
{
    std::string buffer;
    series.saveToBuffer(buffer, 0);

    std::filesystem::path parentDir = savePath.parent_path();
    if (! std::filesystem::exists(parentDir))
    {
        std::filesystem::create_directories(parentDir);
    }
    Antares::IO::fileSetContent(savePath.string(), buffer);
}

bool generateThermalTimeSeries(Data::Study& study,
                               const std::vector<Data::ThermalCluster*>& clusters,
                               const std::string& savePath)
{
    logs.info();
    logs.info() << "Generating the thermal time-series";

    auto generator = AvailabilityTSgenerator(study,
                                             study.parameters.nbTimeSeriesThermal,
                                             study.runtime->random[Data::seedTsGenThermal]);

    for (auto* cluster: clusters)
    {
        cluster->series.timeSeries.reset(study.parameters.nbTimeSeriesThermal, HOURS_PER_YEAR);
        AvailabilityTSGeneratorData tsGenerationData(cluster);
        generator.run(tsGenerationData);
    }

    bool archive = study.parameters.timeSeriesToArchive & Data::timeSeriesThermal;
    bool doWeWrite = archive && !study.parameters.noOutput;
    if (! doWeWrite)
    {
        logs.info() << "Study parameters forbid writing thermal TS.";
        return true;
    }

    for (auto* cluster: clusters)
    {
        auto areaName = cluster->parentArea->id.to<std::string>();
        auto clusterName = cluster->id();
        auto filePath = fs::path(savePath) / areaName / clusterName += ".txt";

        writeTStoDisk(cluster->series.timeSeries, filePath.string());
    }

    return true;
}

// gp : we should try to add const identifiers before args here
bool generateLinkTimeSeries(std::vector<LinkTSgenerationParams>& links,
                            StudyParamsForLinkTS& generalParams,
                            const std::string& savePath)
{
    logs.info();
    logs.info() << "Generation of links time-series";

    auto generator = AvailabilityTSgenerator(generalParams.derated,
                                             generalParams.nbLinkTStoGenerate,
                                             generalParams.random);
    for (auto& link: links)
    {
        if (! link.hasValidData)
        {
            logs.error() << "Missing data for link " << link.namesPair.first << "/" << link.namesPair.second;
            return false;
        }

        if (link.forceNoGeneration)
            continue; // Skipping the link

        Data::TimeSeriesNumbers fakeTSnumbers; // gp : to quickly get rid of
        Data::TimeSeries ts(fakeTSnumbers);
        ts.resize(generalParams.nbLinkTStoGenerate, HOURS_PER_YEAR);

        // DIRECT
        AvailabilityTSGeneratorData tsConfigDataDirect(link, ts, link.modulationCapacityDirect, link.namesPair.second);

        generator.run(tsConfigDataDirect);

        std::string filePath = savePath + SEP + link.namesPair.first + SEP + link.namesPair.second
                               + "_direct.txt";
        writeTStoDisk(ts.timeSeries, filePath);

        // INDIRECT
        AvailabilityTSGeneratorData tsConfigDataIndirect(link, ts, link.modulationCapacityIndirect, link.namesPair.second);

        generator.run(tsConfigDataIndirect);

        filePath = savePath + SEP + link.namesPair.first + SEP + link.namesPair.second
                               + "_indirect.txt";
        writeTStoDisk(ts.timeSeries, filePath);
    }

    return true;
}
} // namespace Antares::TSGenerator
