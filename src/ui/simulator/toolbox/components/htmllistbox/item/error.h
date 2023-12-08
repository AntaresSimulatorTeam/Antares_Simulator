/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ERROR_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ERROR_H__

#include "item.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
class Error : public IItem
{
public:
    Error(const wxString& text, bool warning = false);
    virtual ~Error();

    virtual wxString htmlContent(const wxString& searchString);

public:
    const wxString pText;
    const bool pWarning;
};

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#include "item.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_H__
