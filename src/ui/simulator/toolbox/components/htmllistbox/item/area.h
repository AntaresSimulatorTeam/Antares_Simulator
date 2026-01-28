// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_AREA_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_AREA_H__

#include "item.h"
#include <antares/study/study.h>

#include <memory>

namespace Antares::Component::HTMLListbox::Item
{
/*!
** \brief Single item for an Input (abstract)
**
** An input is often an item list (list of areas, list of interconnections...)
** but this list may be heterogenous (for example `areas + groups`)
** This class a merely a simple wrapper to bring commons display operations
*/
class Area: public IItem
{
public:
    //! The smartptr
    using Ptr = std::shared_ptr<Area>;

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

} // namespace Antares::Component::HTMLListbox::Item

#include "area.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_AREA_H__
