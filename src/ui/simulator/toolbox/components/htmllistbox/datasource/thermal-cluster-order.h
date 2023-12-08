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

#pragma once

#include "cluster-order.h"
#include <map>
#include <list>

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
using ThermalClusterList = std::list<Data::ThermalCluster*>;
using ThermalClusterMap = std::map<wxString, ThermalClusterList>;

class ThermalClustersByOrder : public ClustersByOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    ThermalClustersByOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~ThermalClustersByOrder();
    //@}

private:
    virtual void sortClustersInGroup(ThermalClusterList& clusterList) = 0;

    void reorderItemsList(const wxString& search) override;
    void rebuildItemsList(const wxString& search) override;

}; // ThermalClustersByOrder

class ThermalClustersByAlphaOrder : public ThermalClustersByOrder
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

private:
    void sortClustersInGroup(ThermalClusterList& clusterList) override;

}; // class ThermalClustersByAlphaOrder

class ThermalClustersByAlphaReverseOrder : public ThermalClustersByOrder
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

private:
    void sortClustersInGroup(ThermalClusterList& clusterList) override;

}; // class ThermalClustersByAlphaReverseOrder

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
