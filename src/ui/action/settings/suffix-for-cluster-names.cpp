// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <action/settings/suffix-for-cluster-names.h>

namespace Antares::Action::Settings
{
void SuffixPlantName::prepareSkipWL(Context& ctx)
{
    // reset the property
    ctx.property["cluster.name.suffix"].clear();
}

bool SuffixPlantName::prepareWL(Context& ctx)
{
    // reset the property
    ctx.property["cluster.name.suffix"] = pValue;

    if (pValue.empty())
    {
        pInfos.caption = "Suffix for thermal cluster names";
        pInfos.state = stDisabled;
    }
    else
    {
        pInfos.message.clear();
        pInfos.state = stReady;
        if (pValue == "<auto>")
        {
            pInfos.caption = "Suffix for thermal cluster names (auto)";
            pInfos.message << "Make sure there is no collision between thermal cluster names";
        }
        else
        {
            pInfos.caption = "Suffix for thermal cluster names (custom)";
            pInfos.message << "All thermal cluster names will be suffixed with \"" << pValue << '"';
        }
    }
    return true;
}

void SuffixPlantName::behaviorToText(Behavior behavior, Yuni::String& out)
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
