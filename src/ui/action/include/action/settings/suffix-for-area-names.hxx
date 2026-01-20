// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXAREA_NAME_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXAREA_NAME_HXX__

namespace Antares::Action::Settings
{
template<class StringT>
inline SuffixAreaName::SuffixAreaName(const StringT& value, bool enabled):
    pValue(value)
{
    pInfos.caption = "Suffix for area names";
    if (!enabled)
    {
        pInfos.behavior = bhSkip;
    }
}

inline SuffixAreaName::~SuffixAreaName()
{
}

inline bool SuffixAreaName::performWL(Context&)
{
    return true;
}

inline bool SuffixAreaName::autoExpand() const
{
    return false;
}

inline bool SuffixAreaName::shouldPrepareRootNode() const
{
    return true;
}

} // namespace Antares::Action::Settings

#endif // __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXAREA_NAME_HXX__
