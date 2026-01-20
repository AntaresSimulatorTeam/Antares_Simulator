// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include "item.h"
#include <antares/study/study.h>

#include <memory>

namespace Antares::Component::HTMLListbox::Item
{
/*!
** \brief Single item for an abstract cluster
**
** See parent classes for more explanations
*/
class ClusterItem: public IItem
{
public:
    // Default constructor
    ClusterItem(Antares::Data::Cluster* a);

    // Additional Additional HTML content ("<td>my text</td>")
    ClusterItem(Antares::Data::Cluster* a, const wxString& additional);

    // Destructor
    virtual ~ClusterItem();

    bool interactive() const override
    {
        return true;
    }

    // Get the HTML Content for the item
    wxString htmlContent(const wxString& searchString);

    virtual void addAdditionalIcons(wxString& out) const = 0;

protected:
    /*!
    ** \brief Add a portion of Html from a cluster to a string
    **
    ** A very simple equivalent html code would be :
    ** \code
    ** <td>cluster name</td>
    ** \endcode
    **
    ** \param[in,out] out The string where the result will be appended
    ** \param cluster The cluster
    ** \param searchString The string to highlight
    ** return True if something has been highlighted, false otherwise
    */
    bool HtmlContent(wxString& out, Data::Cluster* cluster, const wxString& searchString);

    wxString getIconFilePath(const AnyString& filename);
    void preloadImages();

private:
    virtual wxString htmlContentTail() = 0;

protected:
    // The current generic cluster
    Antares::Data::Cluster* pCluster;

private:
    // Additional text
    const wxString pText;
    wxString pIconFileEnabled;
    wxString pIconFileDisabled;

}; // class ClusterItem

} // namespace Antares::Component::HTMLListbox::Item
