// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_SOLVER_timeSeries_GENERATOR_H__
#define __ANTARES_SOLVER_timeSeries_GENERATOR_H__

#include <yuni/yuni.h>

#include <antares/series/series.h>
#include <antares/study/fwd.h>
#include <antares/study/parameters.h>
#include <antares/study/parts/thermal/cluster.h>
#include <antares/study/study.h>

#include "xcast/xcast.h"

namespace fs = std::filesystem;
using LinkPair = std::pair<std::string, std::string>;
using LinkPairs = std::vector<LinkPair>;

namespace Antares::TSGenerator
{

struct StudyParamsForLinkTS
{
    unsigned int nbLinkTStoGenerate = 1;
    bool derated = false;
    // gp : we will have a problem with that if seed-tsgen-links not set in
    // gp : generaldata.ini. In that case, our default value is wrong.
    MersenneTwister random;
};

struct LinkTSgenerationParams
{
    LinkPair namesPair;

    unsigned unitCount = 1;
    double nominalCapacity = 0;

    double forcedVolatility = 0.;
    double plannedVolatility = 0.;

    Data::StatisticalLaw forcedLaw = Data::LawUniform;
    Data::StatisticalLaw plannedLaw = Data::LawUniform;

    std::unique_ptr<Data::PreproAvailability> prepro;

    Matrix<> modulationCapacityDirect;
    Matrix<> modulationCapacityIndirect;

    bool forceNoGeneration = false;
    bool hasValidData = true;
};

class AvailabilityTSGeneratorData final
{
public:
    explicit AvailabilityTSGeneratorData(Data::ThermalCluster*);

    AvailabilityTSGeneratorData(LinkTSgenerationParams&,
                                Matrix<>& modulation,
                                const std::string& name);

    const unsigned& unitCount;
    const double& nominalCapacity;

    const double& forcedVolatility;
    const double& plannedVolatility;

    Data::StatisticalLaw& forcedLaw;
    Data::StatisticalLaw& plannedLaw;

    Data::PreproAvailability* prepro;

    Matrix<>::ColumnType& modulationCapacity;

    const std::string& name;
};

using listOfLinks = std::vector<Data::AreaLink*>;

void ResizeGeneratedTimeSeries(Data::AreaList& areas, Data::Parameters& params);

/*!
** \brief Regenerate the time-series
*/
template<enum Data::TimeSeriesType T>
bool GenerateTimeSeries(Data::Study& study, IResultWriter& writer);

bool generateThermalTimeSeries(Data::Study& study,
                               const std::vector<Data::ThermalCluster*>& clusters,
                               MersenneTwister& thermalRandom);

void writeThermalTimeSeries(const std::vector<Data::ThermalCluster*>& clusters,
                            const fs::path& savePath);

bool generateLinkTimeSeries(std::vector<LinkTSgenerationParams>& links,
                            StudyParamsForLinkTS&,
                            const fs::path& savePath);

std::vector<Data::ThermalCluster*> getAllClustersToGen(const Data::AreaList& areas,
                                                       bool globalThermalTSgeneration);

/*!
** \brief Destroy all TS Generators
*/
void DestroyAll(Data::Study& study);

/*!
** \brief Destroy a TS generator if it exists and no longer needed
*/
template<enum Data::TimeSeriesType T>
void Destroy(Data::Study& study);

} // namespace Antares::TSGenerator

#include "generator.hxx"

#endif // __ANTARES_SOLVER_timeSeries_GENERATOR_H__
