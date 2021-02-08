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
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_HXX__

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
inline Component& Component::operator+=(Item::IItem::Ptr it)
{
    this->add(it);
    return *this;
}

inline Component& Component::operator<<(Item::IItem::Ptr it)
{
    this->add(it);
    return *this;
}

inline Component& Component::operator+=(Item::IItem* it)
{
    this->add(it);
    return *this;
}

inline Component& Component::operator<<(Item::IItem* it)
{
    this->add(it);
    return *this;
}

template<class D>
inline D* Component::addDatasource()
{
    D* d = new D(*this);
    internalAddDatasource(d);
    if (!pCurrentDatasource)
        pCurrentDatasource = d;
    return d;
}

inline Datasource::IDatasource* Component::datasource()
{
    return pCurrentDatasource;
}

inline bool Component::empty() const
{
    return pItems.empty();
}

inline uint Component::size() const
{
    return (uint)pItems.size();
}

} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_HXX__
