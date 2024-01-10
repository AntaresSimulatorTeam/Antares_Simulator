/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
