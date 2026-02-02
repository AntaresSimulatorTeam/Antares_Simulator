// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/variable/dynamicAggregation/setData.h"

namespace Antares::Solver::Variable
{

SetDataSingleYear::SetDataSingleYear(const std::set<Data::Area*, Data::CompareAreaName>& set):
    SetDataBase(set)
{
    for (unsigned int i = 0; i < thermalGroupNames_.size(); ++i)
    {
        thermalResults_.emplace_back(HOURS_PER_YEAR, 0);
    }
    for (unsigned int i = 0; i < renewableGroupNames_.size(); ++i)
    {
        renewableResults_.emplace_back(HOURS_PER_YEAR, 0);
    }
    for (unsigned int i = 0; i < stsGroupNames_.size(); ++i)
    {
        stsInjectionResults_.emplace_back(HOURS_PER_YEAR, 0);
        stsWithdrawalResults_.emplace_back(HOURS_PER_YEAR, 0);
        stsLevelResults_.emplace_back(HOURS_PER_YEAR, 0);
    }
}

void SetDataSingleYear::addResultsToSet(const PROBLEME_HEBDO& pb)
{
    for (const auto* area: set_)
    {
        // Thermal clusters
        for (const auto& cluster: area->thermal.list.each_enabled_and_not_mustrun())
        {
            const std::string& groupName = cluster->getGroup();
            size_t index = thermalGroupToNumbers_[groupName];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                thermalResults_[index][realHour] += pb.ResultatsHoraires[area->index]
                                                      .ProductionThermique[h]
                                                      .ProductionThermiqueDuPalier[cluster->index];
            }
        }
        // Renewable clusters
        for (const auto& cluster: area->renewable.list.each_enabled())
        {
            const std::string& groupName = cluster->getGroup();
            size_t index = renewableGroupToNumbers_[groupName];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                renewableResults_[index][realHour] += cluster->valueAtTimeStep(pb.year, realHour);
            }
        }
        // STS clusters
        uint clusterIndex = 0;
        for (const auto& sts: area->shortTermStorage.storagesByIndex)
        {
            const std::string& groupName = sts.properties.groupName;
            size_t index = stsGroupToNumbers_[groupName];

            const auto& stsResults = pb.ResultatsHoraires[area->index]
                                       .ShortTermStorage[clusterIndex];

            for (unsigned h = 0; h < Constants::nbHoursInAWeek; ++h)
            {
                unsigned realHour = h + pb.HeureDansLAnnee;
                stsInjectionResults_[index][realHour] += stsResults.injection[h];
                stsWithdrawalResults_[index][realHour] += stsResults.withdrawal[h];
                stsLevelResults_[index][realHour] += stsResults.level[h];
            }
            ++clusterIndex;
        }
    }
}

void SetDataSingleYear::processGroups(const std::vector<std::vector<long double>>& results,
                                      const std::set<std::string>& groupNames,
                                      const Category::Precision& precision,
                                      const std::string& suffix,
                                      Data::Study& study,
                                      SurveyResults& survey,
                                      bool doWeAverage) const
{
    size_t index = 0;

    for (const auto& group: groupNames)
    {
        survey.variableCaption = group + suffix;
        survey.variableUnit = "MWh";

        IntermediateValues values;
        values.initializeFromStudy(study);
        values.reset();
        std::ranges::copy(results[index], values.hour);

        // average is only used for STS level
        doWeAverage ? values.computeAveragesForCurrentYearFromHourlyResults()
                    : values.computeStatisticsForTheCurrentYear();

        values.buildAnnualSurveyReport<VCardDynamic>(survey, Category::FileLevel::va, precision);
        ++index;
    }
}

void SetDataSingleYear::appendToSurvey(SurveyResults& survey,
                                       Category::Precision precision,
                                       Data::Study& study) const
{
    processGroups(thermalResults_, thermalGroupNames_, precision, "_TH_PROD", study, survey);
    processGroups(renewableResults_, renewableGroupNames_, precision, "_RES_PROD", study, survey);
    processGroups(stsInjectionResults_, stsGroupNames_, precision, "_INJECTION", study, survey);
    processGroups(stsWithdrawalResults_, stsGroupNames_, precision, "_WITHDRAWAL", study, survey);
    processGroups(stsLevelResults_, stsGroupNames_, precision, "_LEVEL", study, survey, true);
}

} // namespace Antares::Solver::Variable
