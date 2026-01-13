/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
                             const Data::Parameters& params,
                             unsigned int numSpace,
                             IResultWriter& resultWriter);
    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    unsigned int numSpace_ = 0;
    const Data::Parameters& params_;
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
                                        const Date::Calendar& calendar);

    void execute(const optRuntimeData& opt_runtime_data) override;

private:
    const AreaList& area_list_;
    const Date::Calendar& calendar_;
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

    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;
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

} // namespace Antares::Solver::Simulation
