// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_INSPECTOR_DATA_HXX__
#define __ANTARES_WINDOWS_INSPECTOR_DATA_HXX__

namespace Antares::Window::Inspector
{
inline void InspectorData::determineEmpty()
{
    empty = areas.empty() and links.empty() and ThClusters.empty() and RnClusters.empty()
            and constraints.empty() and studies.empty();
}

} // namespace Antares::Window::Inspector

#endif // __ANTARES_WINDOWS_INSPECTOR_DATA_H__
