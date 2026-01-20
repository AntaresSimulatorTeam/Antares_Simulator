// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_RENEWABLE_CLUSTER_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_RENEWABLE_CLUSTER_H__

#include "cluster-item.h"

#include <memory>

namespace Antares::Component::HTMLListbox::Item
{
/*!
** \brief Single item for a renewable cluster.
**
** See parent classes for more explanations
*/
class RenewableClusterItem: public ClusterItem
{
public:
    using Ptr = std::shared_ptr<RenewableClusterItem>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    RenewableClusterItem(Antares::Data::RenewableCluster* a);
    /*!
    ** \brief additional Additional HTML content ("<td>my text</td>")
    */
    RenewableClusterItem(Antares::Data::RenewableCluster* a, const wxString& additional);
    //! Destructor
    virtual ~RenewableClusterItem();
    //@}

    //! Get the attached cluster
    Antares::Data::RenewableCluster* renewableAggregate() const;

    void addAdditionalIcons(wxString& out) const override;

private:
    wxString htmlContentTail() override;

private:
    //! The current RenewableCluster
    Antares::Data::RenewableCluster* pRenewableCluster;

}; // class RenewableClusterItem

} // namespace Antares::Component::HTMLListbox::Item

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_RENEWABLE_CLUSTER_H__
