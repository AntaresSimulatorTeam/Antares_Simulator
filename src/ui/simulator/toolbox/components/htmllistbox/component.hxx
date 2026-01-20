// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_HXX__

namespace Antares::Component::HTMLListbox
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
    {
        pCurrentDatasource = d;
    }
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

} // namespace Antares::Component::HTMLListbox

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_HXX__
