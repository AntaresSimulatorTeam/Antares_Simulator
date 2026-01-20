// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "misc-gen.h"

namespace Antares::Action::AntaresStudy::Area
{
MiscGen::MiscGen(const AnyString& areaname):
    pOriginalAreaName(areaname)
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

} // namespace Antares::Action::AntaresStudy::Area
