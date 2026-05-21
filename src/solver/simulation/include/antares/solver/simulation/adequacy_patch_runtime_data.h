// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <memory>
#include <set>
#include <vector>

#include <antares/study/fwd.h>
#include <antares/study/study.h>

using adqPatchParamsMode = Antares::Data::AdequacyPatch::AdequacyPatchMode;

// Forward declaration — full type defined in gems-csr-adapter.h
namespace Antares::AdequacyPatch
{
class GemsCsrAdapter;
}

class AdequacyPatchRuntimeData
{
private:
    std::vector<std::set<int>> csrTriggeredHoursPerArea_;

public:
    explicit AdequacyPatchRuntimeData() = default;
    AdequacyPatchRuntimeData(const AdequacyPatchRuntimeData&) = default;
    AdequacyPatchRuntimeData(AdequacyPatchRuntimeData&&) = default;
    AdequacyPatchRuntimeData& operator=(const AdequacyPatchRuntimeData&) = default;
    AdequacyPatchRuntimeData& operator=(AdequacyPatchRuntimeData&&) = default;
    // Destructor is defined out-of-line so that GemsCsrAdapter can remain an incomplete type here.
    ~AdequacyPatchRuntimeData();

    AdequacyPatchRuntimeData(const Antares::Data::AreaList& areas,
                             const std::vector<Antares::Data::AreaLink*>& links);

    std::vector<adqPatchParamsMode> areaMode;
    std::vector<adqPatchParamsMode> originAreaMode;
    std::vector<adqPatchParamsMode> extremityAreaMode;
    std::vector<double> hurdleCostCoefficients;

    // GEMS flow-based constraints fed into the CSR sub-problem.
    // Set to true and assign gemsCsrAdapter at simulation init (step 5).
    bool useGemsFbConstraints = false;
    std::shared_ptr<Antares::AdequacyPatch::GemsCsrAdapter> gemsCsrAdapter;

    bool wasCSRTriggeredAtAreaHour(int area, int hour) const;
    void addCSRTriggeredAtAreaHour(int area, int hour);
};
