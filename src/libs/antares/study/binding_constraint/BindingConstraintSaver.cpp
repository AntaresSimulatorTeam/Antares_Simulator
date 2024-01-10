/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
//
// Created by marechaljas on 16/05/23.
//
#include "BindingConstraintSaver.h"
#include "BindingConstraint.h"
#include <yuni/yuni.h>
#include <string>
#include "antares/study/area/area.h"
#include "antares/study/fwd.h"

namespace Antares::Data {

using namespace Yuni;
bool BindingConstraintSaver::saveToEnv(EnvForSaving& env, const BindingConstraint * bindingConstraint)
{
    env.section->add("name", bindingConstraint->pName);
    env.section->add("id", bindingConstraint->pID);
    env.section->add("enabled", bindingConstraint->pEnabled);
    env.section->add("type", BindingConstraint::TypeToCString(bindingConstraint->pType));
    env.section->add("operator", BindingConstraint::OperatorToCString(bindingConstraint->pOperator));
    env.section->add("filter-year-by-year", datePrecisionIntoString(bindingConstraint->pFilterYearByYear));
    env.section->add("filter-synthesis", datePrecisionIntoString(bindingConstraint->pFilterSynthesis));
    env.section->add("group", bindingConstraint->group());

    if (!bindingConstraint->pComments.empty())
        env.section->add("comments", bindingConstraint->pComments);

    if (!bindingConstraint->pLinkWeights.empty())
    {
        auto end = bindingConstraint->pLinkWeights.end();
        for (auto i = bindingConstraint->pLinkWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid link");
            assert(i->first->from and "Invalid area name");
            assert(i->first->with and "Invalid area name");

            const AreaLink &lnk = *(i->first);
            env.key.clear() << lnk.from->id << '%' << lnk.with->id;
            String value;
            value << i->second;
            if (bindingConstraint->pLinkOffsets.find(i->first) != bindingConstraint->pLinkOffsets.end())
                value << '%' << bindingConstraint->pLinkOffsets.at(i->first);
            env.section->add(env.key, value);
        }
    }

    if (!bindingConstraint->pClusterWeights.empty())
    {
        auto end = bindingConstraint->pClusterWeights.end();
        for (auto i = bindingConstraint->pClusterWeights.begin(); i != end; ++i)
        {
            // asserts
            assert(i->first and "Invalid thermal cluster");

            const ThermalCluster &clstr = *(i->first);
            env.key.clear() << clstr.getFullName();
            String value;
            value << i->second;
            if (bindingConstraint->pClusterOffsets.find(i->first) != bindingConstraint->pClusterOffsets.end())
                value << '%' << bindingConstraint->pClusterOffsets.at(i->first);
            env.section->add(env.key, value);
        }
    }

    // Exporting the matrix
    env.matrixFilename.clear() << bindingConstraint->timeSeriesFileName(env);
    return bindingConstraint->RHSTimeSeries_.saveToCSVFile(env.matrixFilename.c_str());
}
}