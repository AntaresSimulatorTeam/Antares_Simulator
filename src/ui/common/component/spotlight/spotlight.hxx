// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_HXX__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_HXX__

#include <wx/dc.h>

namespace Antares::Component
{
inline void Spotlight::IProvider::redoResearch()
{
    if (pSpotlightComponent)
    {
        pSpotlightComponent->redoResearch();
    }
}

inline Spotlight::IProvider::Ptr Spotlight::provider() const
{
    return pDataProvider;
}

inline bool Spotlight::IProvider::onSelect(IItem::Ptr&)
{
    return true;
}

inline bool Spotlight::IProvider::onDoubleClickSelect(IItem::Ptr&)
{
    return true;
}

inline bool Spotlight::IProvider::onSelect(const IItem::Vector&)
{
    return true;
}

inline Spotlight* Spotlight::IProvider::component() const
{
    return pSpotlightComponent;
}

inline uint Spotlight::itemHeight() const
{
    return pItemHeight;
}

} // namespace Antares::Component

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_H__
