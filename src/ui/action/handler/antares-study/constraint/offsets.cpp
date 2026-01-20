// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "offsets.h"
#include <antares/utils/utils.h>

using namespace Yuni;

namespace Antares::Action::AntaresStudy::Constraint
{
Offsets::Offsets(const AnyString& name, Antares::Data::ConstraintName targetName):
    pOriginalConstraintName(name),
    targetName(targetName),
    pCurrentContext(nullptr)
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
    {
        out = original;
    }
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
            const std::function<void(Data::AreaName&, const Data::AreaName&)> tr = std::bind(
              targetName.empty() ? &Offsets::translate : &Offsets::toLower,
              this,
              std::placeholders::_1,
              std::placeholders::_2);

            ctx.constraint->copyOffsets(*ctx.study, *source, true, tr);
            pCurrentContext = nullptr;
            return true;
        }
    }
    return false;
}

} // namespace Antares::Action::AntaresStudy::Constraint
