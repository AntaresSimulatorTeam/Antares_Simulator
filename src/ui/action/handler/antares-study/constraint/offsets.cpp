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

#include "offsets.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Constraint
{
Offsets::Offsets(const AnyString& name, Antares::Data::ConstraintName targetName) :
 pOriginalConstraintName(name), targetName(targetName), pCurrentContext(NULL)
{
    pInfos.caption << "Offsets";
}

Offsets::~Offsets()
{
}

bool Offsets::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The Offsets will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }
    return true;
}

void Offsets::translate(Antares::Data::AreaName& out, const Antares::Data::AreaName& original)
{
    assert(pCurrentContext);
    assert(not original.empty());

    auto i = pCurrentContext->areaLowerNameMapping.find(original);
    if (i != pCurrentContext->areaLowerNameMapping.end())
    {
        out = i->second;
        out.toLower();
    }
    else
        out = original;
    assert(not out.empty());
    logs.debug() << "  copy/paste: binding constraint offset : " << original << " -> " << out;
}

void Offsets::toLower(Antares::Data::AreaName& out, const Antares::Data::AreaName& original)
{
    out = original;
    out.toLower();

    assert(not out.empty());
    logs.debug() << "  copy/paste: binding constraint weight : " << original << " -> " << out;
}

bool Offsets::performWL(Context& ctx)
{
    if (ctx.constraint && ctx.extStudy)
    {
        assert(not pOriginalConstraintName.empty());

        Antares::Data::ConstraintName id;
        TransformNameIntoID(pOriginalConstraintName, id);

        auto source = ctx.extStudy->bindingConstraints.find(id);

        if (source && source != ctx.constraint)
        {
            pCurrentContext = &ctx;
            Bind<void(Data::AreaName&, const Data::AreaName&)> tr;
            if (!targetName.empty())
            {
                tr.bind(this, &Offsets::toLower);
            }
            else
            {
                tr.bind(this, &Offsets::translate);
            }
            ctx.constraint->copyOffsets(*ctx.study, *source, true, tr);
            pCurrentContext = nullptr;
            return true;
        }
    }
    return false;
}

} // namespace Constraint
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares
