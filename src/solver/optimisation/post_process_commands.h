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

class HydroLevelsUpdatePostProcessCmd : public basePostProcessCommand
{
public:
    HydroLevelsUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                    AreaList& areas,
                                    bool remixWasRun,
                                    bool computeAnyway);
    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    bool remixWasRun_ = false;
    bool computeAnyway_ = false;
};

class RemixHydroPostProcessCmd : public basePostProcessCommand
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
public:
    DTGmarginForAdqPatchPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                       AreaList& areas,
                                       unsigned int thread_number);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
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
public:
    CurtailmentSharingPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                     AreaList& areas,
                                     unsigned int thread_number);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    double calculateDensNewAndTotalLmrViolation();
    std::vector<double> calculateENSoverAllAreasForEachHour() const;
    std::set<int> identifyHoursForCurtailmentSharing(std::vector<double> sumENS) const;
    std::set<int> getHoursRequiringCurtailmentSharing() const;

    const AreaList& area_list_;
    unsigned int thread_number_ = 0;
};

} // namespace Antares::Solver::Simulation
