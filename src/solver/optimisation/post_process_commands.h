#pragma once

#include <antares/study.h>
#include <antares/study/fwd.h>

#include "post_process_command.h"

namespace Antares::Solver::Simulation
{
class DispatchableMarginPostProcessCmd : public PostProcessCommand
{
public:
    DispatchableMarginPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                     unsigned int thread_number,
                                     Data::AreaList& areas);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    unsigned int thread_number_ = 0;
    const Data::AreaList& area_list_;
    unsigned int hourInYear_ = 0;
};

class HydroLevelsUpdatePostProcessCmd : public PostProcessCommand
{
public:
    HydroLevelsUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                    Data::AreaList& areas,
                                    bool remixWasRun,
                                    bool computeAnyway);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const Data::AreaList& area_list_;
    bool remixWasRun_ = false;
    bool computeAnyway_ = false;
};

class RemixHydroPostProcessCmd : public PostProcessCommand
{
public:
    RemixHydroPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                             Data::AreaList& areas,
                             Data::SheddingPolicy sheddingPolicy,
                             Data::SimplexOptimization simplexOptimization,
                             unsigned int thread_number);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const Data::AreaList& area_list_;
    unsigned int thread_number_ = 0;
    Data::SheddingPolicy shedding_policy_;
    Data::SimplexOptimization splx_optimization_;
    unsigned int hourInYear_ = 0;
};

class DTGmarginForAdqPatchPostProcessCmd : public PostProcessCommand
{
public:
    DTGmarginForAdqPatchPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                       Data::AreaList& areas,
                                       unsigned int thread_number);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const Data::AreaList& area_list_;
    unsigned int thread_number_ = 0;
};

class InterpolateWaterValuePostProcessCmd : public PostProcessCommand
{
public:
    InterpolateWaterValuePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                        Data::AreaList& areas,
                                        const Date::Calendar& calendar);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const Data::AreaList& area_list_;
    const Date::Calendar& calendar_;
    unsigned int hourInYear_ = 0;
};

class HydroLevelsFinalUpdatePostProcessCmd : public PostProcessCommand
{
public:
    HydroLevelsFinalUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo, Data::AreaList& areas);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const Data::AreaList& area_list_;
};
} // namespace Antares::Solver::Simulation
