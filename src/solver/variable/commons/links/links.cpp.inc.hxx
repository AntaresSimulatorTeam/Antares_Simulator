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

#include "../../variable.h"
#include <antares/study/area/area.h>
#include "../../setofareas.h"

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace LINK_NAMESPACE
{
Links::~Links()
{
    // Releasing the memory occupied by the areas
    delete[] pLinks;
}

void Links::initializeFromArea(Data::Study* study, Data::Area* area)
{
    // Assert
    assert(study && "A study pointer must not be null");
    assert(area && "An area pointer must not be null");

    pLinkCount = (uint)area->links.size();
    if (pLinkCount)
    {
        pLinks = new NextType[pLinkCount];

        // For each link...
        uint lnkIndex = 0;
        auto end = area->links.end();
        for (auto i = area->links.begin(); i != end; ++i, ++lnkIndex)
        {
            // Instancing a new set of variables of the area
            NextType& n = pLinks[lnkIndex];

            // Initialize the variables
            // From the study
            n.initializeFromStudy(*study);
            // From the area
            n.initializeFromArea(study, area);
            // From the link
            n.initializeFromAreaLink(study, i->second);
        }
    }
    else
        pLinks = nullptr;
}

void Links::simulationBegin()
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].simulationBegin();
    }
}

void Links::simulationEnd()
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].simulationEnd();
    }
}

void Links::buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
{
    int count_int = count;
    bool linkDataLevel = dataLevel & Category::link;
    bool areaDataLevel = dataLevel & Category::area;
    if (count_int && (linkDataLevel || areaDataLevel))
    {
        if (not results.data.area->links.empty())
        {
            auto end = results.data.area->links.cend();
            for (auto i = results.data.area->links.cbegin(); i != end; ++i)
            {
                results.data.link = i->second;
                pLinks[results.data.link->indexForArea].buildDigest(
                  results, digestLevel, Category::link);
            }
        }
    }
}

void Links::beforeYearByYearExport(uint year, uint numSpace)
{
    for (uint i = 0; i != pLinkCount; ++i)
        pLinks[i].beforeYearByYearExport(year, numSpace);
}

} // namespace LINK_NAMESPACE
} // namespace Variable
} // namespace Solver
} // namespace Antares
