#pragma once

#include "../simulation/base_post_process.h"

namespace Antares::Solver::Simulation
{
class DispatchableMarginPostProcessCmd : public basePostProcessCommand
{
public:
    DispatchableMarginPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                     unsigned int thread_number,
                                     AreaList& areas);
    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    unsigned int thread_number_ = 0;
    const AreaList& area_list_;
};

class RemixHydroPostProcessCmd: public basePostProcessCommand
{
public:
    RemixHydroPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                             AreaList& areas,
                             SheddingPolicy sheddingPolicy,
                             SimplexOptimization simplexOptimization,
                             unsigned int thread_number);
    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    unsigned int thread_number_ = 0;
    SheddingPolicy shedding_policy_;
    SimplexOptimization splx_optimization_;
};

class DTGmarginForAdqPatchPostProcessCmd : public basePostProcessCommand
{
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

public:
    DTGmarginForAdqPatchPostProcessCmd(const AdqPatchParams& adqPatchParams,
                                       PROBLEME_HEBDO* problemeHebdo,
                                       AreaList& areas,
                                       unsigned int thread_number);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AdqPatchParams& adqPatchParams_;
    const AreaList& area_list_;
    unsigned int thread_number_ = 0;
};

class InterpolateWaterValuePostProcessCmd : public basePostProcessCommand
{
public:
    InterpolateWaterValuePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                        AreaList& areas,
                                        const Date::Calendar& calendar);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    const Date::Calendar& calendar_;
};

class HydroLevelsFinalUpdatePostProcessCmd : public basePostProcessCommand
{
public:
    HydroLevelsFinalUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo, AreaList& areas);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
};

class CurtailmentSharingPostProcessCmd : public basePostProcessCommand
{
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

public:
    CurtailmentSharingPostProcessCmd(const AdqPatchParams& adqPatchParams,
                                     PROBLEME_HEBDO* problemeHebdo,
                                     AreaList& areas,
                                     unsigned int thread_number);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    double calculateDensNewAndTotalLmrViolation();
    std::vector<double> calculateENSoverAllAreasForEachHour() const;
    std::set<int> identifyHoursForCurtailmentSharing(const std::vector<double>& sumENS) const;
    std::set<int> getHoursRequiringCurtailmentSharing() const;

    const AreaList& area_list_;
    const AdqPatchParams& adqPatchParams_;
    unsigned int thread_number_ = 0;
};

} // namespace Antares::Solver::Simulation
