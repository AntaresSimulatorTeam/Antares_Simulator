/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <antares/study/memory-usage.h>
#include "../../variable.h"
#include "../../area.h"
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

            // Flush all memory into the swap files
            // (only if the support is available)
            if (Antares::Memory::swapSupport)
                Antares::memory.flushAll();
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
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
    }
}

void Links::simulationEnd()
{
    for (uint i = 0; i != pLinkCount; ++i)
    {
        pLinks[i].simulationEnd();
        if (Antares::Memory::swapSupport)
            Antares::memory.flushAll();
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

    if (Antares::Memory::swapSupport)
        Antares::memory.flushAll();
}

} // namespace LINK_NAMESPACE
} // namespace Variable
} // namespace Solver
} // namespace Antares
