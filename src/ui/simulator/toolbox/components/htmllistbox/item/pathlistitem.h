// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_PATHLIST_ITEM_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_PATHLIST_ITEM_H__

#include "item.h"
#include <antares/paths/list.h>

#include <memory>

namespace Antares::Component::HTMLListbox::Item
{
/*!
** \brief Single item for an Input
*/
class PathListItem: public IItem
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

} // namespace Antares::Component::HTMLListbox::Item

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_PATHLIST_ITEM_H__
