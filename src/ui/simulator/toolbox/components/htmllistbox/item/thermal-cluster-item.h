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
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_THERMAL_CLUSTER_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_THERMAL_CLUSTER_H__

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
** \brief Single item for a thermal cluster.
**
** See parent classes for more explanations
*/
class ThermalClusterItem : public ClusterItem
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

} // namespace Item
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_ITEM_THERMAL_CLUSTER_H__
