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

#include "weights.h"
#include "antares/utils/utils.h"

using namespace Yuni;

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Constraint
{
Weights::Weights(const AnyString& name, Antares::Data::ConstraintName targetName) :
 pOriginalConstraintName(name), targetName(targetName), pCurrentContext(nullptr)
{
    pInfos.caption << "Weights";
}

Weights::~Weights()
{
}

bool Weights::prepareWL(Context&)
{
    pInfos.message.clear();
    pInfos.state = stReady;
    switch (pInfos.behavior)
    {
    case bhOverwrite:
        pInfos.message << "The Weights will be copied";
        break;
    default:
        pInfos.state = stNothingToDo;
        break;
    }
    return true;
}

void Weights::translate(Antares::Data::AreaName& out, const Antares::Data::AreaName& original)
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
    logs.debug() << "  copy/paste: binding constraint weight : " << original << " -> " << out;
}

void Weights::toLower(Antares::Data::AreaName& out, const Antares::Data::AreaName& original)
{
    out = original;
    out.toLower();

    assert(not out.empty());
    logs.debug() << "  copy/paste: binding constraint weight : " << original << " -> " << out;
}

bool Weights::performWL(Context& ctx)
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
            const std::function<void(Data::AreaName&, const Data::AreaName&)> tr
              = std::bind(targetName.empty() ? &Weights::translate : &Weights::toLower,
                          this,
                          std::placeholders::_1,
                          std::placeholders::_2);
            ctx.constraint->copyWeights(*ctx.study, *source, true, tr);
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
