// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "standard_variable.h"

namespace Antares::Solver::Variable::Economy
{

struct ReservoirLevelTraits
{
    static constexpr std::string_view kCaption = "H. LEV";
    static constexpr std::string_view kUnit = "%";
    static constexpr std::string_view kDescription = "Hydro Level";
    static constexpr uint8_t decimal = 2;
    static constexpr uint8_t isPossiblyNonApplicable = 1;
    static constexpr uint8_t spatialAggregate = Category::noSpatialAggregate;

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
class ReservoirLevelSetters
{
public:
    void setReservoirCapacity(double capacity)
    {
        pReservoirCapacity = capacity;
    }

protected:
    double pReservoirCapacity = 0.;
};

template<class NextT = Container::EndOfList>
class ReservoirLevel: public StandardVariableBase<ReservoirLevel<NextT>,
                                                  NextT,
                                                  VCardStandardVariable<ReservoirLevelTraits>>,
                      public ReservoirLevelSetters<NextT>
{
public:
    using BaseType = StandardVariableBase<ReservoirLevel<NextT>,
                                          NextT,
                                          VCardStandardVariable<ReservoirLevelTraits>>;
    using NextType = NextT;

    void initializeFromArea(Data::Study* study, Data::Area* area)
    {
        this->setReservoirCapacity(area->hydro.reservoirCapacity);
        BaseType::initializeFromArea(study, area);
    }

    void hourForEachArea(State& state, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace][state.hourInTheYear] = state.hourlyResults
                                                                          ->niveauxHoraires
                                                                            [state.hourInTheWeek]
                                                                        / this->pReservoirCapacity
                                                                        * 100.;
        NextT::hourForEachArea(state, numSpace);
    }

    void yearEnd(unsigned int year, unsigned int numSpace)
    {
        this->pValuesForTheCurrentYear[numSpace].computeAveragesForCurrentYearFromHourlyResults();
        NextT::yearEnd(year, numSpace);
    }
};

} // namespace Antares::Solver::Variable::Economy
