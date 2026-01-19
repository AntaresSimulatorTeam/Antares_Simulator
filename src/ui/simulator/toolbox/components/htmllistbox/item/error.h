// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ERROR_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ERROR_H__

#include "item.h"

namespace Antares::Component::HTMLListbox::Item
{
class Error: public IItem
{
public:
    Error(const wxString& text, bool warning = false);
    virtual ~Error();

    virtual wxString htmlContent(const wxString& searchString);

public:
    const wxString pText;
    const bool pWarning;
};

} // namespace Antares::Component::HTMLListbox::Item

#include "item.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_H__
