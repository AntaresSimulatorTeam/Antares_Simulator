// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_INFO_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_INFO_H__

#include "item.h"

namespace Antares::Component::HTMLListbox::Item
{
class Info: public IItem
{
public:
    Info(const wxString& text);
    virtual ~Info();

    virtual wxString htmlContent(const wxString& searchString);

public:
    wxString pText;
};

} // namespace Antares::Component::HTMLListbox::Item

#include "item.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_H__
