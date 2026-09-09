// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/simulation/base_post_process.h"

// Kept in Antares::Solver::Simulation (not the optim subtree): these are simulation
// post-processing commands, not optimization-model code.
namespace Antares::Solver::Simulation
{
class DispatchableMarginPostProcessCmd final: public basePostProcessCommand
{
public:
    DispatchableMarginPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                     unsigned int numSpace,
                                     AreaList& areas);
    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    unsigned int numSpace_ = 0;
    const AreaList& area_list_;
};

class RemixHydroPostProcessCmd final: public basePostProcessCommand
{
public:
    RemixHydroPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                             AreaList& areas,
                             const Parameters& params,
                             unsigned int numSpace,
                             IResultWriter& resultWriter);
    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    unsigned int numSpace_ = 0;
    const Parameters& params_;
    IResultWriter& resultWriter_;
};

class UpdateMrgPriceAfterCSRcmd final: public basePostProcessCommand
{
public:
    UpdateMrgPriceAfterCSRcmd(PROBLEME_HEBDO* problemeHebdo,
                              AreaList& areas,
                              unsigned int numSpace);
    void execute(const optRuntimeData&) override;

private:
    const AreaList& area_list_;
    unsigned int numSpace_ = 0;
};

class DTGnettingAfterCSRcmd final: public basePostProcessCommand
{
public:
    DTGnettingAfterCSRcmd(PROBLEME_HEBDO* problemeHebdo, AreaList& areas, unsigned int numSpace);
    void execute(const optRuntimeData&) override;

private:
    const AreaList& area_list_;
    unsigned int numSpace_ = 0;
};

class InterpolateWaterValuePostProcessCmd final: public basePostProcessCommand
{
public:
    InterpolateWaterValuePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                        AreaList& areas,
                                        const Calendar& calendar);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    const Calendar& calendar_;
};

class HydroLevelsFinalUpdatePostProcessCmd final: public basePostProcessCommand
{
public:
    HydroLevelsFinalUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo, AreaList& areas);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
};

class CurtailmentSharingPostProcessCmd final: public basePostProcessCommand
{
public:
    CurtailmentSharingPostProcessCmd(const AdqPatchParams& adqPatchParams,
                                     PROBLEME_HEBDO* problemeHebdo,
                                     AreaList& areas,
                                     unsigned int numSpace,
                                     const OptimizationOptions& solverOptions);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    double calculateDensNewAndTotalLmrViolation();
    std::vector<double> calculateENSoverAllAreasForEachHour() const;
    std::set<int> identifyHoursForCurtailmentSharing(const std::vector<double>& sumENS) const;
    std::set<int> getHoursRequiringCurtailmentSharing() const;

    using AdqPatchParams = AdequacyPatch::AdqPatchParams;
    const AreaList& area_list_;
    const AdqPatchParams& adqPatchParams_;
    unsigned int numSpace_ = 0;
    const OptimizationOptions& solverOptions_;
};

class WriteDebugAdequacyPatch final: public basePostProcessCommand
{
public:
    WriteDebugAdequacyPatch(PROBLEME_HEBDO* problemeHebdo,
                            AreaList& areas,
                            unsigned int numSpace,
                            IResultWriter& writer,
                            std::string fileLabel);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    void writeAreaData(const optRuntimeData& opt_runtime_data);
    void writeLinkData(const optRuntimeData& opt_runtime_data);

    const AreaList& areas_;
    unsigned int numSpace_ = 0;
    IResultWriter& writer_;
    const std::string fileLabel_;
};

// Writes the state of the legacy results into the simulation table stage named
// `stage`, at the point of the post-process list where it is inserted. Placed
// right after the command whose effect it is meant to capture, so the stage
// name matches that post-process.
//
// Does nothing when `tables` is null, i.e. when the run does not write
// simulation tables at all.
class DumpSimulationTablePostProcessCmd final: public basePostProcessCommand
{
public:
    DumpSimulationTablePostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                      std::string stage,
                                      IO::Outputs::OptimisationsSimulationTable* tables);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const std::string stage_;
    IO::Outputs::OptimisationsSimulationTable* const tables_;
};

} // namespace Antares::Solver::Simulation
