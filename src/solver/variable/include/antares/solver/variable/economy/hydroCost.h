// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct HydroCostTraits
{
    static constexpr std::string_view kCaption = "H. COST";
    static constexpr std::string_view kUnit = "Euro";
    static constexpr std::string_view kDescription = "Hydro Cost throughout all MC years, of all "
                                                     "the thermal dispatchable clusters";
    static constexpr uint8_t decimal = 0;
    static constexpr uint8_t isPossiblyNonApplicable = 0;
    static constexpr uint8_t spatialAggregate = Category::spatialAggregateSum;

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
};

template<class NextT = Container::EndOfList>
class HydroCostSetters
{
public:
    void setPumpRatio(double ratio)
    {
        pPumpRatio = ratio;
    }

protected:
    double pPumpRatio = 0.;
};

template<class NextT = Container::EndOfList>
class HydroCost
    : public StandardVariableBase<HydroCost<NextT>, NextT, VCardStandardVariable<HydroCostTraits>>,
      public HydroCostSetters<NextT>
{
public:
    using BaseType = StandardVariableBase<HydroCost<NextT>,
                                          NextT,
                                          VCardStandardVariable<HydroCostTraits>>;
    using NextType = NextT;

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        this->setPumpRatio(area->hydro.pumpingEfficiency);
        BaseType::initializeFromArea(study, area);
    }

    void simulationBegin()
    {
        for (unsigned int numSpace = 0; numSpace < this->pNbYearsParallel; numSpace++)
        {
            this->pValuesForTheCurrentYear[numSpace].reset();
        }
        NextT::simulationBegin();
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace].hour[state.hourInTheYear]
          += state.hourlyResults->valeurH2oHoraire[state.hourInTheWeek]
             * (state.hourlyResults->TurbinageHoraire[state.hourInTheWeek]
                - this->pPumpRatio * state.hourlyResults->PompageHoraire[state.hourInTheWeek]);
        NextT::hourForEachArea(state, numSpace);
    }

private:
    using BaseType::pNbYearsParallel;
    using BaseType::pValuesForTheCurrentYear;
};

} // namespace Antares::Solver::Variable::Economy
