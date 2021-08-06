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

#pragma once

#include "item.h"

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
        //! The smartptr
        // typedef Yuni::SmartPtr<ClusterItem> Ptr;

    public:
        // Default constructor
        ClusterItem();
        
        // Additional Additional HTML content ("<td>my text</td>")
        ClusterItem(const wxString& additional);

        // Destructor
        virtual ~ClusterItem();

        virtual bool interactive() const
        {
            return true;
        }

    protected:
        wxString getIconFilePath(const AnyString& filename);
        void preloadImages();

    private:
        /*!
        ** \brief Add a portion of Html from a thermal cluster to a string
        **
        ** A very simple equivalent html code would be :
        ** \code
        ** <td>thermal cluster name</td>
        ** \endcode
        **
        ** \param[in,out] out The string where the result will be appended
        ** \param th The thermal cluster
        ** \param searchString The string to highlight
        ** return True if something has been highlighted, false otherwise
        */
        // static bool HtmlContent(wxString& out, Data::ThermalCluster* th, const wxString& searchString);

        virtual wxString getClusterIconFilePath() = 0;

    protected:
        //! Additional text
        const wxString pText;
        wxString pIconFileEnabled;
        wxString pIconFileDisabled;
        wxString pClusterIconFilePath;

    }; // class ClusterItem


} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
