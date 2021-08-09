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

// #include "datasource.h"
// #include <yuni/core/event.h>
// #include <antares/study.h>
#include "cluster-order.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{

class ThermalClustersByAlphaOrder : public ClustersByAlphaOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    ThermalClustersByAlphaOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~ThermalClustersByAlphaOrder();
    //@}

    wxString name() const override
    {
        return wxT("Thermal clusters in alphabetical order");
    }
    const char* icon() const override
    {
        return "images/16x16/sort_alphabet.png";
    }
    virtual void refresh(const wxString& search = wxEmptyString) override;

}; // class ThermalClustersByAlphaOrder

class ThermalClustersByAlphaReverseOrder : public ClustersByAlphaReverseOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    ThermalClustersByAlphaReverseOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~ThermalClustersByAlphaReverseOrder();
    //@}

    virtual wxString name() const
    {
        return wxT("Thermal clusters in reverse alphabetical order");
    }
    virtual const char* icon() const
    {
        return "images/16x16/sort_alphabet_descending.png";
    }
    virtual void refresh(const wxString& search = wxEmptyString) override;

}; // class ThermalClustersByAlphaReverseOrder

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
