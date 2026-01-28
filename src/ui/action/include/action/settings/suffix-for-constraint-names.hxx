// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXCONSTRAINT_NAME_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXCONSTRAINT_NAME_HXX__

namespace Antares::Action::Settings
{
template<class StringT>
inline SuffixConstraintName::SuffixConstraintName(const StringT& value, bool enabled):
    pValue(value)
{
    pInfos.caption = "Suffix for constraint names";
    if (!enabled)
    {
        pInfos.behavior = bhSkip;
    }
}

inline SuffixConstraintName::~SuffixConstraintName()
{
}

inline bool SuffixConstraintName::performWL(Context&)
{
    return true;
}

inline bool SuffixConstraintName::autoExpand() const
{
    return false;
}

inline bool SuffixConstraintName::shouldPrepareRootNode() const
{
    return true;
}

} // namespace Antares::Action::Settings

#endif // __ANTARES_LIBS_STUDY_ACTION_SETTINGS_SUFFIXCONSTRAINT_NAME_HXX__
