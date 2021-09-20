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
#ifndef __ANTARES_WINDOWS_INSPECTOR_INSPECTOR_H__
#define __ANTARES_WINDOWS_INSPECTOR_INSPECTOR_H__

#include <yuni/yuni.h>
#include <antares/study/study.h>

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

//! \name Study
//@{
/*!
** \brief Clear the selection then Add a study
*/
void SelectStudy(const Data::Study* study);
/*!
** \brief Add a study into the selection
*/
void AddStudy(const Data::Study* study);
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
** \brief Clear the selection then Add a vector of areas
*/
void SelectAreas(const Data::Area::Set& areas);

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
** \brief Clear the selection then Add a vector of thermal clusters
*/
void SelectThermalClusters(const Data::ThermalCluster::Vector& clusters);

/*!
** \brief Add a thermal cluster to the selection
*/
void AddThermalCluster(const Data::ThermalCluster* cluster);

/*!
** \brief Add an array of clusters to the current selection
*/
void AddThermalClusters(const Data::ThermalCluster::Vector& clusters);

/*!
** \brief Add a set of clusters to the current selection
*/
void AddThermalClusters(const Data::ThermalCluster::Set& clusters);

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

//! \name Data::Binding constraints
//@{
/*!
** \brief Clear the selection then Add a binding constraint
*/
void SelectBindingConstraint(const Data::BindingConstraint* constraint);

/*!
** \brief Clear the selection then Add a vector of binding constraints
*/
void SelectBindingConstraints(const Data::BindingConstraint::Vector& array);

/*!
** \brief Add a binding constraint to the selection
*/
void AddBindingConstraint(const Data::BindingConstraint* constraint);

/*!
** \brief Add a set of binding constraints to the current selection
*/
void AddBindingConstraints(const Data::BindingConstraint::Set& set);

/*!
** \brief Remove a binding constraint from the selection
*/
void RemoveBindingConstraint(const Data::BindingConstraint* constraint);
//@}

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

bool IsThermalClusterSelected(const Data::AreaName& area, const Data::ClusterName& name);

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
