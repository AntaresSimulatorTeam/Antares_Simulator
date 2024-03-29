/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_HXX__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_HXX__

#include <wx/dc.h>

namespace Antares
{
namespace Component
{
inline void Spotlight::IProvider::redoResearch()
{
    if (pSpotlightComponent)
        pSpotlightComponent->redoResearch();
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

} // namespace Component
} // namespace Antares

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_H__
