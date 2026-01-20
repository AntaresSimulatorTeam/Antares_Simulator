// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <action/settings/decal-area-position.h>

namespace Antares::Action::Settings
{
DecalAreaPosition::DecalAreaPosition(bool enabled):
    pValueX("25"),
    pValueY("-20")
{
    pInfos.caption = "Decal area coordinates";
    if (!enabled)
    {
        pInfos.behavior = bhSkip;
    }
}

void DecalAreaPosition::prepareSkipWL(Context& ctx)
{
    // reset the property
    ctx.property["area.coordinate.x.offset"].clear();
    ctx.property["area.coordinate.y.offset"].clear();
}

bool DecalAreaPosition::prepareWL(Context& ctx)
{
    ctx.property["area.coordinate.x.offset"] = pValueX;
    ctx.property["area.coordinate.y.offset"] = pValueY;

    if (!pValueX && !pValueY)
    {
        pInfos.state = stDisabled;
    }
    else
    {
        pInfos.message.clear();
        pInfos.state = stReady;
        if (not pValueX.empty())
        {
            pInfos.message << "x: +" << pValueX << "   ";
        }
        if (not pValueY.empty())
        {
            pInfos.message << "y: +" << pValueY;
        }
    }
    return true;
}

void DecalAreaPosition::behaviorToText(Behavior behavior, Yuni::String& out)
{
    switch (behavior)
    {
    case bhOverwrite:
        out = "enabled";
        break;
    case bhMerge:
        out = "enabled";
        break;
    case bhSkip:
        out = "skip";
        break;
    case bhMax:
        out.clear();
        break;
    }
}

} // namespace Antares::Action::Settings
