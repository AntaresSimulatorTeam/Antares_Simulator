
#ifndef __SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationByLTStorage_H__
#define __SOLVER_VARIABLE_ECONOMY_VCardReserveParticipationByLTStorage_H__

#include "../storage/results.h"

namespace Antares {
namespace Solver {
namespace Variable {
namespace Economy {

struct VCardReserveParticipationByLTStorage {
    static std::string Caption() { return "LT STORAGE PARTICIPATION TO RESERVE"; }
    static std::string Unit() { return "MWh"; }
    static std::string Description() { return "Reserve Participation from long-term storage to a reserve"; }

    typedef Results<R::AllYears::Average<>> ResultsType;
    typedef VCardReserveParticipationByLTStorage VCardForSpatialAggregate;

    enum {
        categoryDataLevel = Category::area,
        categoryFileLevel = ResultsType::categoryFile & (Category::de),
        precision = Category::all,
        nodeDepthForGUI = +0,
        decimal = 0,
        columnCount = Category::dynamicColumns,
        spatialAggregate = Category::spatialAggregateSum,
        spatialAggregateMode = Category::spatialAggregateEachYear,
        spatialAggregatePostProcessing = 0,
        hasIntermediateValues = 1,
        isPossiblyNonApplicable = 0,
    };

    typedef IntermediateValues IntermediateValuesDeepType;
    typedef IntermediateValues* IntermediateValuesBaseType;
    typedef IntermediateValuesBaseType* IntermediateValuesType;
};

} // namespace Economy
} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif