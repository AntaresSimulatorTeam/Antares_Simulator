/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_PATHLIST_ITEM_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_PATHLIST_ITEM_H__

#include "item.h"
#include <antares/paths/list.h>

#include <memory>

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Item
{
/*!
** \brief Single item for an Input
*/
class PathListItem : public IItem
{
public:
    //! The smartptr
    using Ptr = std::shared_ptr<PathListItem>;

public:
    static bool HtmlContent(wxString& out,
                            wxString name,
                            const wxString& searchString,
                            bool isFolder,
                            uint64_t size);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    PathListItem(const Yuni::Clob& a, const PathList::FileInfo& opts);
    //! Destructor
    virtual ~PathListItem()
    {
    }
    //@}

    /*!
    ** \brief Get the HTML Content for the item
    */
    virtual wxString htmlContent(const wxString& searchString);

    virtual bool interactive() const
    {
        return true;
    }

protected:
    //! Additional text
    const wxString pText;
    uint64_t pSize;
    bool pIsFolder;

}; // class PathListItem

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_PATHLIST_ITEM_H__
