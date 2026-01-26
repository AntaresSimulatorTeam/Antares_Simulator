// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <action/settings/suffix-for-constraint-names.h>

namespace Antares::Action::Settings
{
void SuffixConstraintName::prepareSkipWL(Context& ctx)
{
    // reset the property
    ctx.property["constraint.name.suffix"].clear();
}

bool SuffixConstraintName::prepareWL(Context& ctx)
{
    // reset the property
    ctx.property["constraint.name.suffix"] = pValue;

    if (pValue.empty())
    {
        pInfos.caption = "Suffix for constraint names";
        pInfos.state = stDisabled;
    }
    else
    {
        pInfos.message.clear();
        pInfos.state = stReady;
        if (pValue == "<auto>")
        {
            pInfos.caption = "Suffix for constraint names (auto)";
            pInfos.message << "Make sure there is no collision between constraint names";
        }
        else
        {
            pInfos.caption = "Suffix for constraint names (custom)";
            pInfos.message << "All constraint names will be suffixed with \"" << pValue << '"';
        }
    }
    return true;
}

void SuffixConstraintName::behaviorToText(Behavior behavior, Yuni::String& out)
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
