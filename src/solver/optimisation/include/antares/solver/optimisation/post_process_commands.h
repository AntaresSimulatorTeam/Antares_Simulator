/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include "antares/solver/simulation/base_post_process.h"

namespace Antares::Solver::Simulation
{
class DispatchableMarginPostProcessCmd: public basePostProcessCommand
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

class HydroLevelsUpdatePostProcessCmd: public basePostProcessCommand
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

class RemixHydroPostProcessCmd: public basePostProcessCommand
{
public:
    RemixHydroPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                             AreaList& areas,
                             SheddingPolicy sheddingPolicy,
                             SimplexOptimization simplexOptimization,
                             unsigned int numSpace);
    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    unsigned int numSpace_ = 0;
    SheddingPolicy shedding_policy_;
    SimplexOptimization splx_optimization_;
};

class DTGmarginForAdqPatchPostProcessCmd: public basePostProcessCommand
{
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

public:
    DTGmarginForAdqPatchPostProcessCmd(PROBLEME_HEBDO* problemeHebdo,
                                       AreaList& areas,
                                       unsigned int numSpace);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    unsigned int numSpace_ = 0;
};

class InterpolateWaterValuePostProcessCmd: public basePostProcessCommand
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

class HydroLevelsFinalUpdatePostProcessCmd: public basePostProcessCommand
{
public:
    HydroLevelsFinalUpdatePostProcessCmd(PROBLEME_HEBDO* problemeHebdo, AreaList& areas);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
};

class CurtailmentSharingPostProcessCmd: public basePostProcessCommand
{
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

public:
    CurtailmentSharingPostProcessCmd(const AdqPatchParams& adqPatchParams,
                                     PROBLEME_HEBDO* problemeHebdo,
                                     AreaList& areas,
                                     unsigned int numSpace);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    double calculateDensNewAndTotalLmrViolation();
    std::vector<double> calculateENSoverAllAreasForEachHour() const;
    std::set<int> identifyHoursForCurtailmentSharing(const std::vector<double>& sumENS) const;
    std::set<int> getHoursRequiringCurtailmentSharing() const;

    const AreaList& area_list_;
    const AdqPatchParams& adqPatchParams_;
    unsigned int numSpace_ = 0;
};

} // namespace Antares::Solver::Simulation
