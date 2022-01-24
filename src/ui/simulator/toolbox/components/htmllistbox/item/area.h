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
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_AREA_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_AREA_H__

#include "item.h"
#include <antares/study.h>

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
** \brief Single item for an Input (abstract)
**
** An input is often an item list (list of areas, list of interconnections...)
** but this list may be heterogenous (for example `areas + groups`)
** This class a merely a simple wrapper to bring commons display operations
*/
class Area : public IItem
{
public:
    //! The smartptr
    typedef std::shared_ptr<Area> Ptr;

public:
    /*!
    ** \brief Add a portion of Html from an area to a string
    **
    ** A very simple equivalent html code would be :
    ** \code
    ** <td>area name</td>
    ** \endcode
    **
    ** \param[in,out] out The string where the result will be appended
    ** \param area The area
    ** \param searchString The string to highlight
    ** return True if something has been highlighted, false otherwise
    */
    static bool HtmlContent(wxString& out, Data::Area* area, const wxString& searchString);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Area(Data::Area* a);
    /*!
    ** \brief additional Additional HTML content ("<td>my text</td>")
    */
    Area(Data::Area* a, const wxString& additional);
    //! Destructor
    virtual ~Area();
    //@}

    /*!
    ** \brief Get the HTML Content for the item
    */
    virtual wxString htmlContent(const wxString& searchString);

    //! Get the attached area
    Data::Area* area() const;

    virtual bool interactive() const
    {
        return true;
    }

protected:
    //! The current Area
    Data::Area* pArea;
    //! Additional text
    const wxString pText;

}; // class Area

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#include "area.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_AREA_H__
