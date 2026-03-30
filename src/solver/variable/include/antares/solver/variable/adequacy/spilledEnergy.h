// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ADEQUACY_SpilledEnergy_H__
#define __SOLVER_VARIABLE_ADEQUACY_SpilledEnergy_H__

#include "antares/solver/variable/adequacy/adequacy_standard_variable.h"
#include "antares/solver/variable/variable.h"

namespace Antares::Solver::Variable::Adequacy
{

struct VCardSpilledEnergy: public AdequacyVariableTraits<VCardSpilledEnergy>
{
    static constexpr std::string_view kCaption = "SPIL. ENRG";
    static constexpr std::string_view kUnit = "MWh";
    static constexpr std::string_view kDescription = "Spilled Energy (generation that cannot be "
                                                     "satisfied)";
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

    using ResultsType = Results<
      R::AllYears::Average<R::AllYears::StdDeviation<R::AllYears::Min<R::AllYears::Max<>>>>>;

    using VCardForSpatialAggregate = VCardSpilledEnergy;
    using IntermediateValuesBaseType = IntermediateValues;
    using IntermediateValuesType = std::vector<IntermediateValues>;
    using IntermediateValuesTypeForSpatialAg = std::unique_ptr<IntermediateValuesBaseType[]>;

    static double getValue(const State& state, unsigned int)
    {
        return +state.hourlyResults->ValeursHorairesDeDefaillanceNegative[state.hourInTheWeek]
               + state.resSpilled.entry[state.area->index][state.hourInTheWeek];
    }
};

template<class NextT = Container::EndOfList>
class SpilledEnergy: public AdequacyVariableBase<SpilledEnergy<NextT>, NextT, VCardSpilledEnergy>
{
public:
    using BaseType = AdequacyVariableBase<SpilledEnergy<NextT>, NextT, VCardSpilledEnergy>;
    using VCardType = VCardSpilledEnergy;

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
        assert(state.hourlyResults && "Invalid pointer to simplex results");
        this->pValuesForTheCurrentYear[numSpace][state.hourInTheYear] = VCardType::getValue(
          state,
          numSpace);
        NextT::hourForEachArea(state, numSpace);
    }
};

} // namespace Antares::Solver::Variable::Adequacy

#endif
