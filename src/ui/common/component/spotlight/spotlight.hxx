/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
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
