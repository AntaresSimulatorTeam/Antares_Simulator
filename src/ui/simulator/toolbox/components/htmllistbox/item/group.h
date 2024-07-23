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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_GROUP_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_GROUP_H__

#include "item.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
class Group : public IItem
{
public:
    Group(const wxString& text);
    virtual ~Group();

    virtual wxString htmlContent(const wxString& searchString);

public:
    wxString pText;
};

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#include "item.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_GROUP_H__
