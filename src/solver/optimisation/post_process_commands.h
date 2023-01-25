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
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    unsigned int thread_number_ = 0;
    const AreaList& area_list_;
    unsigned int hourInYear_ = 0;
};


class HydroLevelsUpdatePostProcessCmd : public basePostProcessCommand
{
public:
    HydroLevelsUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                    AreaList& areas,
                                    bool remixWasRun,
                                    bool computeAnyway);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const AreaList& area_list_;
    bool remixWasRun_ = false;
    bool computeAnyway_ = false;
};


class RemixHydroPostProcessCmd : public basePostProcessCommand
{
public:
    RemixHydroPostProcessCmd(
        PROBLEME_HEBDO* problemeHebdo,
        AreaList& areas,
        SheddingPolicy sheddingPolicy,
        SimplexOptimization simplexOptimization,
        unsigned int thread_number);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const AreaList& area_list_;
    unsigned int thread_number_ = 0;
    SheddingPolicy shedding_policy_;
    SimplexOptimization splx_optimization_;
    unsigned int hourInYear_ = 0;

};


class DTGmarginForAdqPatchPostProcessCmd : public basePostProcessCommand
{  
public:
    DTGmarginForAdqPatchPostProcessCmd(
        PROBLEME_HEBDO* problemeHebdo,
        AreaList& areas, 
        unsigned int thread_number);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const AreaList& area_list_;
    unsigned int thread_number_ = 0;
};


class InterpolateWaterValuePostProcessCmd : public basePostProcessCommand
{
public:
    InterpolateWaterValuePostProcessCmd(
        PROBLEME_HEBDO* problemeHebdo, 
        AreaList& areas,
        const Date::Calendar& calendar);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const AreaList& area_list_;
    const Date::Calendar& calendar_;
    unsigned int hourInYear_ = 0;
};

class HydroLevelsFinalUpdatePostProcessCmd : public basePostProcessCommand
{
public:
    HydroLevelsFinalUpdatePostProcessCmd(
        PROBLEME_HEBDO* problemeHebdo,
        AreaList& areas);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const AreaList& area_list_;
};

class CurtailmentSharingPostProcessCmd : public basePostProcessCommand
{
public:
    CurtailmentSharingPostProcessCmd(PROBLEME_HEBDO* problemeHebdo, AreaList& areas);
    void acquireOptRuntimeData(const struct optRuntimeData& opt_runtime_data) override;
    void run() override;

private:
    const AreaList& area_list_;
    unsigned int year_ = 0;
    unsigned int week_ = 0;
};

} // namespace Antares::Solver::Simulation