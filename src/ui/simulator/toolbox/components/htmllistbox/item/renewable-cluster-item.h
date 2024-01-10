/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
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
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_RENEWABLE_CLUSTER_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_RENEWABLE_CLUSTER_H__

#include "cluster-item.h"

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
** \brief Single item for a renewable cluster.
**
** See parent classes for more explanations
*/
class RenewableClusterItem : public ClusterItem
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

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_RENEWABLE_CLUSTER_H__
