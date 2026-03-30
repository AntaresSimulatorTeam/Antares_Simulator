// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__
#define __SOLVER_VARIABLE_ADEQUACY_OverallCost_H__

#include "antares/solver/variable/adequacy/adequacy_standard_variable.h"
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Adequacy
{

struct VCardOverallCost: public AdequacyVariableTraits<VCardOverallCost>
{
    static constexpr std::string_view kCaption = "OV. COST";
    static constexpr std::string_view kUnit = "Euro";
    static constexpr std::string_view kDescription = "Overall Cost throughout all MC years";
    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t isPossiblyNonApplicable = 0;

    static std::string Caption()
    {
        return std::string(kCaption);
    }

    static std::string Unit()
    {
        return std::string(kUnit);
    }

    static std::string Description()
    {
        return std::string(kDescription);
    }

    using ResultsType = Results<R::AllYears::Average<>, R::AllYears::Average>;

    using VCardForSpatialAggregate = VCardOverallCost;
    using IntermediateValuesBaseType = IntermediateValues;
    using IntermediateValuesType = std::vector<IntermediateValues>;
    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;
};

template<class NextT = Container::EndOfList>
class OverallCost: public AdequacyVariableBase<OverallCost<NextT>, NextT, VCardOverallCost>
{
public:
    using BaseType = AdequacyVariableBase<OverallCost<NextT>, NextT, VCardOverallCost>;
    using VCardType = VCardOverallCost;

    enum
    {
        count = 1 + NextT::count,
    };

    template<int CDataLevel, int CFile>
    struct Statistics
    {
        enum
        {
            count = BaseType::template Statistics < CDataLevel,
            CFile > ::count,
        };
    };

    void yearEndBuildForEachThermalCluster(State& state, unsigned int year, unsigned int numSpace)
    {
        for (unsigned int i = 0; i < state.study.runtime.rangeLimits.hour[Data::rangeCount]; ++i)
        {
            this->pValuesForTheCurrentYear[numSpace][i] += state
                                                             .thermalClusterOperatingCostForYear[i];
        }
        NextT::yearEndBuildForEachThermalCluster(state, year, numSpace);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        auto area = state.area;
        auto& thermal = state.thermal;

        this->pValuesForTheCurrentYear[numSpace][state.hourInTheYear]
          += (state.hourlyResults->ValeursHorairesDeDefaillancePositive[state.hourInTheWeek]
              * area->thermal.unsuppliedEnergyCost)
             + ((state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek]
                 + state.resSpilled.entry[area->index][state.hourInTheWeek])
                * area->thermal.spilledEnergyCost);

        this->pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear]
          += state.problemeHebdo->CaracteristiquesHydrauliques[state.area->index]
               .WeeklyWaterValueStateRegular
             * (state.hourlyResults->TurbinageHoraire[state.hourInTheWeek]
                - area->hydro.pumpingEfficiency
                    * state.hourlyResults->PompageHoraire[state.hourInTheWeek]);

        for (auto& cluster: area->thermal.list.each_enabled())
        {
            this->pValuesForTheCurrentYear[numSpace][state.hourInTheYear]
              += thermal[area->index].thermalClustersOperatingCost[cluster->enabledIndex];
        }

        NextT::hourForEachArea(state, numSpace);
    }
};

} // namespace Antares::Solver::Variable::Adequacy

#endif
