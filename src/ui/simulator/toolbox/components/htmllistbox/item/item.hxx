// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_HXX__

namespace Antares::Component::HTMLListbox::Item
{
inline bool IItem::highlighted() const
{
    return pHighlighted;
}

inline bool IItem::visible() const
{
    return pVisible;
}

inline void IItem::visible(bool v)
{
    pVisible = v;
}

inline bool IItem::interactive() const
{
    return false;
}

} // namespace Antares::Component::HTMLListbox::Item

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_HXX__
