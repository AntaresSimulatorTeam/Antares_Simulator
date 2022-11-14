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

namespace Antares
{
namespace Component
{
namespace HTMLListbox
{
namespace Datasource
{
using HydroclusterClusterList = std::list<Data::HydroclusterCluster*>;
using HydroclusterClusterMap = std::map<wxString, HydroclusterClusterList>;

class HydroclusterClustersByOrder : public ClustersByOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    HydroclusterClustersByOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~HydroclusterClustersByOrder();
    //@}

private:
    virtual void sortClustersInGroup(HydroclusterClusterList& clusterList) = 0;

    void reorderItemsList(const wxString& search) override;
    void rebuildItemsList(const wxString& search) override;

}; // HydroclusterClustersByOrder

class HydroclusterClustersByAlphaOrder : public HydroclusterClustersByOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    HydroclusterClustersByAlphaOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~HydroclusterClustersByAlphaOrder();
    //@}

    virtual wxString name() const override
    {
        return wxT("Hydrocluster clusters in alphabetical order");
    }

    virtual const char* icon() const override
    {
        return "images/16x16/sort_alphabet.png";
    }

private:
    void sortClustersInGroup(HydroclusterClusterList& clusterList) override;

}; // class HydroclusterClustersByAlphaOrder

class HydroclusterClustersByAlphaReverseOrder : public HydroclusterClustersByOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    HydroclusterClustersByAlphaReverseOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~HydroclusterClustersByAlphaReverseOrder();
    //@}

    virtual wxString name() const
    {
        return wxT("Hydrocluster clusters in reverse alphabetical order");
    }

    virtual const char* icon() const
    {
        return "images/16x16/sort_alphabet_descending.png";
    }

private:
    void sortClustersInGroup(HydroclusterClusterList& clusterList) override;

}; // class HydroclusterClustersByAlphaReverseOrder

} // namespace Datasource
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares
