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

#include <memory>
#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Data;
using namespace Antares::Date;
using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

namespace Antares::Solver::Simulation
{

struct optRuntimeData
{
    optRuntimeData(unsigned int y, unsigned int w, unsigned int h):
        year(y),
        week(w),
        hourInTheYear(h)
    {
    }

    unsigned int year = 0;
    unsigned int week = 0;
    unsigned int hourInTheYear = 0;
};

class basePostProcessCommand
{
public:
    virtual ~basePostProcessCommand() = default;
    virtual void execute(const optRuntimeData& opt_runtime_data) = 0;

protected:
    // Memeber functions
    basePostProcessCommand(PROBLEME_HEBDO* problemeHebdo);

    // Data members
    PROBLEME_HEBDO* problemeHebdo_;
};

class interfacePostProcessList
{
public:
    virtual ~interfacePostProcessList() = default;

    // gp : we should put all these arguments in a structure, so that we pass a structure
    // gp : to the create(...) method, and to underlying calls to constructors.
    // gp : In case we need new data for a new post process, we would not have to change
    // gp : the constructors' signatures of the post process list classes.
    static std::unique_ptr<interfacePostProcessList> create(AdqPatchParams& adqPatchParams,
                                                            PROBLEME_HEBDO* problemeHebdo,
                                                            uint numSpace,
                                                            AreaList& areas,
                                                            SheddingPolicy sheddingPolicy,
                                                            SimplexOptimization splxOptimization,
                                                            Calendar& calendar);
    void runAll(const optRuntimeData& opt_runtime_data);

protected:
    // Member functions
    interfacePostProcessList(PROBLEME_HEBDO* problemeHebdo, uint numSpace);

    // Data mambers
    PROBLEME_HEBDO* const problemeHebdo_ = nullptr;
    const unsigned int numSpace_;
    std::vector<std::unique_ptr<basePostProcessCommand>> post_process_list;
};

} // namespace Antares::Solver::Simulation
