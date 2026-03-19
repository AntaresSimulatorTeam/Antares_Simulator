// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/version.h>
#include "antares/exception/LoadingError.hpp"
#include "antares/study/fwd.h"

namespace Antares::Check
{
void checkStudyVersion(const Data::StudyVersion& version, const AnyString& StudyFolder);

void checkSimplexRangeHydroPricing(Antares::Data::SimplexOptimization optRange,
                                   Antares::Data::HydroPricingMode hpMode);

void checkSimplexRangeUnitCommitmentMode(Antares::Data::SimplexOptimization optRange,
                                         Antares::Data::UnitCommitmentMode ucMode);

void checkSimplexRangeHydroHeuristic(Antares::Data::SimplexOptimization optRange,
                                     const Antares::Data::AreaList& areas);

void checkMinStablePower(bool tsGenThermal, const Antares::Data::AreaList& areas);

void checkFuelCostColumnNumber(const Antares::Data::AreaList& areas);
void checkCO2CostColumnNumber(const Antares::Data::AreaList& areas);

class IncompatibleDailyOptHeuristicForArea final: public Error::LoadingError
{
public:
    explicit IncompatibleDailyOptHeuristicForArea(const Antares::Data::AreaName& name);
};

class InvalidParametersForThermalClusters final: public Error::LoadingError
{
public:
    explicit InvalidParametersForThermalClusters(const std::map<int, std::string>& clusterNames);

private:
    std::string buildMessage(const std::map<int, std::string>& clusterNames) const;
};
} // namespace Antares::Check
