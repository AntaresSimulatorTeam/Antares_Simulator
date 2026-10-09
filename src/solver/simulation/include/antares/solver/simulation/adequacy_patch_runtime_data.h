// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <set>
#include <vector>

#include <antares/study/fwd.h>
#include <antares/study/study.h>

using adqPatchParamsMode = Antares::Data::AdequacyPatch::AdequacyPatchMode;

class AdequacyPatchRuntimeData
{
private:
    std::vector<std::set<int>> csrTriggeredHoursPerArea_;
    std::vector<std::set<int>> ensZeroedByThresholdPerArea_;
    std::vector<std::vector<double>> marginalCostsBeforeAdqPatch_;

public:
    explicit AdequacyPatchRuntimeData() = default;
    AdequacyPatchRuntimeData(const Antares::Data::AreaList& areas,
                             const std::vector<Antares::Data::AreaLink*>& links);

    std::vector<adqPatchParamsMode> areaMode;
    std::vector<adqPatchParamsMode> originAreaMode;
    std::vector<adqPatchParamsMode> extremityAreaMode;
    std::vector<double> hurdleCostCoefficients;

    bool wasCSRTriggeredAtAreaHour(int area, int hour) const;
    void addCSRTriggeredAtAreaHour(int area, int hour);
    void resetCSRTriggeredHours();

    bool wasENSZeroedByThresholdAtAreaHour(int area, int hour) const;
    void addENSZeroedByThresholdAtAreaHour(int area, int hour);
    void resetENSZeroedByThreshold();

    void setMarginalCostBeforeAdqPatch(int area, int hour, double marginalCost);
    double marginalCostBeforeAdqPatch(int area, int hour) const;
};
