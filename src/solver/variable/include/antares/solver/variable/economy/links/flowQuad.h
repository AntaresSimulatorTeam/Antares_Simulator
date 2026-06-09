// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_ECONOMY_FlowQuad_H__
#define __SOLVER_VARIABLE_ECONOMY_FlowQuad_H__

#include <cassert>
#include <cstring>

#include "static_link_base.h"

namespace Antares::Solver::Variable::Economy
{

struct FlowQuadAuxData
{
    Data::AreaLink* link = nullptr;
    uint nbHours = 0;
    std::vector<std::vector<double>>* transitMoyenInterco = nullptr;
};

struct FlowQuadTraits
{
    static std::string Caption()
    {
        return "FLOW QUAD.";
    }

    static std::string Unit()
    {
        return "MWh";
    }

    static std::string Description()
    {
        return "Flow (quad.)";
    }

    using ResultsProfile = Results<std::tuple<R::AllYears::Raw>>;

    using AuxiliaryDataType = FlowQuadAuxData;

    static void onInitializeFromStudy(IntermediateValues&, FlowQuadAuxData& aux, Data::Study& study)
    {
        aux.nbHours = study.runtime.rangeLimits.hour[Data::rangeEnd] + 1;
        aux.transitMoyenInterco = &study.runtime.transitMoyenInterconnexionsRecalculQuadratique;
    }

    static void onInitializeFromAreaLink(IntermediateValues&,
                                         FlowQuadAuxData& aux,
                                         Data::Study*,
                                         Data::AreaLink* link)
    {
        assert(link && "invalid interconnection");
        aux.link = link;
    }

    static void onSimulationBegin(IntermediateValues& iv, FlowQuadAuxData&)
    {
        iv.reset();
    }

    static void loadDataForSimulationEnd(IntermediateValues& iv, const FlowQuadAuxData& aux)
    {
        assert(aux.link && "invalid interconnection");
        (void)::memcpy(iv.hour,
                       (*aux.transitMoyenInterco)[aux.link->index].data(),
                       sizeof(double) * aux.nbHours);
    }

    template<class R>
    static void buildDigest(SurveyResults& results,
                            int digestLevel,
                            int dataLevel,
                            const R& pResults)
    {
        if (dataLevel & Category::DataLevel::link)
        {
            if (digestLevel & Category::digestFlowQuad)
            {
                results.data.matrix
                  .entry[results.data.link->from->index][results.data.link->with->index]
                  = pResults.rawdata().allYears;
                results.data.matrix
                  .entry[results.data.link->with->index][results.data.link->from->index]
                  = -pResults.rawdata().allYears;
            }
        }
    }
};

using FlowQuad = StaticLinkBase<FlowQuadTraits>;

} // namespace Antares::Solver::Variable::Economy

#endif // __SOLVER_VARIABLE_ECONOMY_FlowQuad_H__
