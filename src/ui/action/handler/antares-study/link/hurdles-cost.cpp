/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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

#include "hurdles-cost.h"
#include <antares/study/area/constants.h>

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Link
{
HurdlesCost::HurdlesCost(const AnyString& fromarea, const AnyString& toarea) :
 pOriginalFromAreaName(fromarea), pOriginalToAreaName(toarea)
{
    pInfos.caption << "Hurdles-Cost";
}

HurdlesCost::~HurdlesCost()
{
}

bool HurdlesCost::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The hurdles-cost will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }

    return true;
}

bool HurdlesCost::performWL(Context& ctx)
{
    if (ctx.link && ctx.extStudy)
    {
        Data::AreaName idFrom;
        Data::AreaName idTo;
        TransformNameIntoID(pOriginalFromAreaName, idFrom);
        TransformNameIntoID(pOriginalToAreaName, idTo);

        Data::AreaLink* source;
        if (pOriginalFromAreaName < pOriginalToAreaName)
            source = ctx.extStudy->areas.findLink(idFrom, idTo);
        else
            source = ctx.extStudy->areas.findLink(idTo, idFrom);

        if (source && source != ctx.link)
        {
            source->parameters.forceReload(true);
            ctx.link->parameters.forceReload(true);

            ctx.link->parameters.pasteToColumn(
              (uint)Data::fhlHurdlesCostDirect,
              source->parameters.entry[Data::fhlHurdlesCostDirect]);
            ctx.link->parameters.pasteToColumn(
              (uint)Data::fhlHurdlesCostIndirect,
              source->parameters.entry[Data::fhlHurdlesCostIndirect]);
            ctx.link->useHurdlesCost = source->useHurdlesCost;

            ctx.link->assetType = source->assetType;
            return true;
        }
    }
    return false;
}

} // namespace Link
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
