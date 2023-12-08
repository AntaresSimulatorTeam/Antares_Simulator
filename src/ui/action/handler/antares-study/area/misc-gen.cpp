/*
** Copyright 2007-2023 RTE
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
** SPDX-License-Identifier: MPL 2.0
*/

#include "misc-gen.h"

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Area
{
MiscGen::MiscGen(const AnyString& areaname) : pOriginalAreaName(areaname)
{
    pInfos.caption << "MiscGen";
}

MiscGen::~MiscGen()
{
}

bool MiscGen::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The misc gen. will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool MiscGen::performWL(Context& ctx)
{
    if (ctx.area && ctx.extStudy && ctx.area->ui)
    {
        Data::Area* source = ctx.extStudy->areas.findFromName(pOriginalAreaName);
        // check the pointer + make sure that this is not the same memory area
        if (source)
        {
            if (source != ctx.area)
            {
                ctx.area->miscGen = source->miscGen;
                source->miscGen.unloadFromMemory();
            }
            return true;
        }
    }
    return false;
}

} // namespace Area
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
