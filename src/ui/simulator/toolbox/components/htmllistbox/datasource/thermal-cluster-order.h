// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include "cluster-order.h"
#include <map>
#include <list>

namespace Antares::Component::HTMLListbox::Datasource
{
using ThermalClusterList = std::list<Data::ThermalCluster*>;
using ThermalClusterMap = std::map<wxString, ThermalClusterList>;

class ThermalClustersByOrder: public ClustersByOrder
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

class ThermalClustersByAlphaOrder: public ThermalClustersByOrder
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

class ThermalClustersByAlphaReverseOrder: public ThermalClustersByOrder
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

} // namespace Antares::Component::HTMLListbox::Datasource
