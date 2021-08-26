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

#include "cluster-order.h"
#include <map>
#include <list>
#include "../item/thermal-cluster-item.h"

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{

typedef std::list<Data::ThermalCluster*> ThermalClusterList;
typedef std::map<wxString, ThermalClusterList> ThermalClusterMap;

using namespace Antares::Component::HTMLListbox::Item;

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

    void refresh(const wxString& search = wxEmptyString) override;

private:
    virtual void sortClustersInGroup(ThermalClusterList& clusterList) = 0;

    void reordering_items_list(const wxString& search);
    void rebuilding_items_list(const wxString& search);

protected:
    std::map<wxString, IItem*> groups_to_items_;
    // std::map<Data::ThermalCluster*, std::pair<ThermalClusterItem*, int> > clusters_to_items_;
    std::map<Data::ThermalCluster*, ThermalClusterItem*> clusters_to_items_;



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
    void sortClustersInGroup(ThermalClusterList& clusterList);

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
    void sortClustersInGroup(ThermalClusterList& clusterList);

}; // class ThermalClustersByAlphaReverseOrder

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
