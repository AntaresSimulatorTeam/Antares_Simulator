// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_HXX__
#define __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_HXX__

namespace Antares::Action::Settings
{
inline DecalAreaPosition::~DecalAreaPosition()
{
}

inline bool DecalAreaPosition::performWL(Context&)
{
    return true;
}

inline bool DecalAreaPosition::autoExpand() const
{
    return false;
}

inline bool DecalAreaPosition::shouldPrepareRootNode() const
{
    return true;
}

} // namespace Antares::Action::Settings

#endif // __ANTARES_LIBS_STUDY_ACTION_SETTINGS_DECAL_AREA_POSITION_HXX__
