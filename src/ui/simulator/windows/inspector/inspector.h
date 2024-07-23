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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_WINDOWS_INSPECTOR_INSPECTOR_H__
#define __ANTARES_WINDOWS_INSPECTOR_INSPECTOR_H__

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

namespace Antares
{
namespace Window
{
namespace Inspector
{
//! \name General management
//@{
/*!
** \brief Refresh the inspector panel (if visible)
*/
void Refresh();

/*!
** \brief Show the inspector panel
*/
void Show();

/*!
** \brief Hide the inspector panel
*/
void Hide();

/*!
** \brief Destroy the inspector panel
*/
void Destroy();

/*!
** \brief Unselect all items
*/
void Unselect();
//@}
//! \name Areas
//@{
/*!
** \brief Clear the selection then Add an area
*/
void SelectArea(const Data::Area* area);

/*!
** \brief Clear the selection then Add a vector of areas
*/
void SelectAreas(const Data::Area::Vector& areas);

/*!
** \brief Add an area to the selection
*/
void AddArea(const Data::Area* area);

/*!
** \brief Add an array of areas to the current selection
*/
void AddAreas(const Data::Area::Vector& list);

/*!
** \brief Add a set of areas to the current selection
*/
void AddAreas(const Data::Area::Set& list);

/*!
** \brief Remove an area from the selection
*/
void RemoveArea(const Data::Area* area);
//@}

//! \name Links
//@{
/*!
** \brief Clear the selection then Add a link
*/
void SelectLink(const Data::AreaLink* lnk);

/*!
** \brief Clear the selection then Add a vector of links
*/
void SelectLinks(const Data::AreaLink::Vector& lnks);

/*!
** \brief Add a link to the selection
*/
void AddLink(const Data::AreaLink* link);

/*!
** \brief Add an array of links to the current selection
*/
void AddLinks(const Data::AreaLink::Vector& links);

/*!
** \brief Add a set of links to the current selection
*/
void AddLinks(const Data::AreaLink::Set& links);

/*!
** \brief get the current list of selected links
*/
const Data::AreaLink::Set& getLinks();

/*!
** \brief Remove a link from the selection
*/
void RemoveLink(const Data::AreaLink* link);
//@}

//! \name Thermal clusters
//@{
/*!
** \brief Clear the selection then Add a thermal cluster
*/
void SelectThermalCluster(const Data::ThermalCluster* cluster);

/*!
** \brief Add an array of clusters to the current selection
*/
void AddThermalClusters(const Data::ThermalCluster::Vector& clusters);
/*!
** \brief Remove a thermal cluster from the selection
*/
void RemoveThermalCluster(const Data::ThermalCluster* cluster);
//@}

//! \name Renewable clusters
//@{
/*!
** \brief Clear the selection then Add a renewable cluster
*/
void SelectRenewableCluster(const Data::RenewableCluster* cluster);

/*!
** \brief Remove a renewable cluster from the selection
*/
void RemoveRenewableCluster(const Data::RenewableCluster* cluster);

/*!
** \brief Add a set of binding constraints to the current selection
*/
void AddBindingConstraints(const Data::BindingConstraintsRepository::Set& set);

//! \name Misc
//@{
/*!
** \brief Get the total number of selected areas
*/
uint SelectionAreaCount();

/*!
** \brief Get the total number of selected links
*/
uint SelectionLinksCount();

/*!
** \brief Get the total number of selected thermal clusters
*/
uint SelectionThermalClusterCount();

/*!
** \brief Get the total number of binding constraints that are selected
*/
uint SelectionBindingConstraintCount();

/*!
** \brief Get the total number of selected items
*/
uint SelectionTotalCount();

/*!
** \brief Get if a set of area (represented by their name) is selected in the inspector
*/
bool isAreaSelected(Antares::Data::AreaName name);
bool AreasSelected(const Data::Area::NameSet& set,
                   std::map<Antares::Data::AreaName, Antares::Data::AreaName>& nameMap);

bool isConstraintSelected(const Yuni::String& constraintName);
bool ConstraintsSelected(const std::set<Yuni::String>& set);

bool IsLinkSelected(const Data::AreaName& from, const Data::AreaName& with);
bool LinksSelected(std::map<Data::AreaName, std::map<Data::AreaName, bool>>& set);

void FirstSelectedArea(Data::AreaName& out);

void FirstSelectedAreaLink(Data::AreaLink** link);
//@}

//! \name Clipboard
//@{
/*!
** \brief Copy to clipboard
**
** \return The total number of items which have been copied to the clipboard
*/
uint CopyToClipboard();
//@}

} // namespace Inspector
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_INSPECTOR_INSPECTOR_H__
