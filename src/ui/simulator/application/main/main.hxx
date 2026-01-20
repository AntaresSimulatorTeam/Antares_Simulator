// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_MAIN_MAIN_HXX__
#define __ANTARES_APPLICATION_MAIN_MAIN_HXX__

namespace Antares::Forms
{
inline wxMenuItem* ApplWnd::menuRecentFiles() const
{
    return pMenuFileRecents;
}

inline Map::Component* ApplWnd::map() const
{
    return pMainMap;
}

inline Component::MainPanel* ApplWnd::mainPanel() const
{
    return pMainPanel;
}

inline MainFormData* ApplWnd::data()
{
    return pData;
}

inline const MainFormData* ApplWnd::data() const
{
    return pData;
}

} // namespace Antares::Forms

#endif // __ANTARES_APPLICATION_MAIN_MAIN_HXX__
