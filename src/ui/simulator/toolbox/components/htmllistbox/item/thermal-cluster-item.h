// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_THERMAL_CLUSTER_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_THERMAL_CLUSTER_H__

#include "cluster-item.h"

#include <memory>

namespace Antares::Component::HTMLListbox::Item
{
/*!
** \brief Single item for a thermal cluster.
**
** See parent classes for more explanations
*/
class ThermalClusterItem: public ClusterItem
{
public:
    using Ptr = std::shared_ptr<ThermalClusterItem>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    ThermalClusterItem(Antares::Data::ThermalCluster* a);
    /*!
    ** \brief additional Additional HTML content ("<td>my text</td>")
    */
    ThermalClusterItem(Antares::Data::ThermalCluster* a, const wxString& additional);
    //! Destructor
    virtual ~ThermalClusterItem();
    //@}

    //! Get the attached area
    Antares::Data::ThermalCluster* thermalAggregate() const;

private:
    wxString htmlContentTail() override;

    wxString pIconFileRefresh;
    wxString pIconFileNoRefresh;

    void preloadAdditionalIcons();
    void addAdditionalIcons(wxString& out) const override;

    //! The current ThermalCluster
    Antares::Data::ThermalCluster* pThermalCluster;
}; // class ThermalClusterItem

} // namespace Antares::Component::HTMLListbox::Item

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_THERMAL_CLUSTER_H__
