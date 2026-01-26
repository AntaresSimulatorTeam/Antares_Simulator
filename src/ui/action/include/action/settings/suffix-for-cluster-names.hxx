// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIX_CLUSTER_NAME_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIX_CLUSTER_NAME_HXX__

namespace Antares::Action::Settings
{
template<class StringT>
inline SuffixPlantName::SuffixPlantName(const StringT& value, bool enabled):
    pValue(value)
{
    pInfos.caption = "Suffix for thermal cluster names";

    if (!enabled)
    {
        pInfos.behavior = bhSkip;
    }
}

inline SuffixPlantName::~SuffixPlantName()
{
}

inline bool SuffixPlantName::performWL(Context&)
{
    return true;
}

inline bool SuffixPlantName::autoExpand() const
{
    return false;
}

inline bool SuffixPlantName::shouldPrepareRootNode() const
{
    return true;
}

} // namespace Antares::Action::Settings

#endif // __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIX_CLUSTER_NAME_HXX__
