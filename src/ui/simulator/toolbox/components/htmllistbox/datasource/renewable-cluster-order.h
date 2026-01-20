// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include "cluster-order.h"
#include <map>
#include <list>
// #include "../item/renewable-cluster-item.h"

namespace Antares::Component::HTMLListbox::Datasource
{
using RenewableClusterList = std::list<Data::RenewableCluster*>;
using RenewableClusterMap = std::map<wxString, RenewableClusterList>;

class RenewableClustersByOrder: public ClustersByOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    RenewableClustersByOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~RenewableClustersByOrder();
    //@}

private:
    virtual void sortClustersInGroup(RenewableClusterList& clusterList) = 0;

    void reorderItemsList(const wxString& search) override;
    void rebuildItemsList(const wxString& search) override;

}; // RenewableClustersByOrder

class RenewableClustersByAlphaOrder: public RenewableClustersByOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    RenewableClustersByAlphaOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~RenewableClustersByAlphaOrder();

    //@}

    virtual wxString name() const override
    {
        return wxT("Renewable clusters in alphabetical order");
    }

    virtual const char* icon() const override
    {
        return "images/16x16/sort_alphabet.png";
    }

private:
    void sortClustersInGroup(RenewableClusterList& clusterList) override;

}; // class RenewableClustersByAlphaOrder

class RenewableClustersByAlphaReverseOrder: public RenewableClustersByOrder
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default Constructor
    RenewableClustersByAlphaReverseOrder(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~RenewableClustersByAlphaReverseOrder();

    //@}

    virtual wxString name() const
    {
        return wxT("Renewable clusters in reverse alphabetical order");
    }

    virtual const char* icon() const
    {
        return "images/16x16/sort_alphabet_descending.png";
    }

private:
    void sortClustersInGroup(RenewableClusterList& clusterList) override;

}; // class RenewableClustersByAlphaReverseOrder

} // namespace Antares::Component::HTMLListbox::Datasource
