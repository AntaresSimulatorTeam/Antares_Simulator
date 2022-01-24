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
    typedef std::shared_ptr<PathListItem> Ptr;

public:
    static bool HtmlContent(wxString& out,
                            wxString name,
                            const wxString& searchString,
                            bool isFolder,
                            Yuni::uint64 size);

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
    Yuni::uint64 pSize;
    bool pIsFolder;

}; // class PathListItem

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_PATHLIST_ITEM_H__
