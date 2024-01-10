/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
*/

#pragma once

#include "item.h"
#include <antares/study/study.h>

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
** \brief Single item for an abstract cluster
**
** See parent classes for more explanations
*/
class ClusterItem : public IItem
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

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
