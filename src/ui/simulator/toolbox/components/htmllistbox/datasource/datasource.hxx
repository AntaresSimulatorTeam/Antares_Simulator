// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_DATASOURCE_HXX__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_DATASOURCE_HXX__

namespace Antares::Component::HTMLListbox::Datasource
{
inline IDatasource::IDatasource(HTMLListbox::Component& parent):
    pParent(parent)
{
}

inline IDatasource::~IDatasource()
{
}

} // namespace Antares::Component::HTMLListbox::Datasource

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_HXX__
