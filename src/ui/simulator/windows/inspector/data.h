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
#ifndef __ANTARES_WINDOWS_INSPECTOR_DATA_H__
#define __ANTARES_WINDOWS_INSPECTOR_DATA_H__

#include <yuni/yuni.h>
#include <memory>
#include <antares/study.h>

namespace Antares
{
namespace Window
{
namespace Inspector
{
/*!
** \brief Collection of items to view/edit from an inspector
**
** \note This class should be renamed
*/
class InspectorData final
{
public:
    //! The most suitable smart pointer for the class
    using Ptr = std::shared_ptr<InspectorData>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit InspectorData(Data::Study::Ptr study);
    //! Destructor
    ~InspectorData();
    //@}

    void clear();

    void determineEmpty();

    /*!
    ** \brief Get the total number of items in this set
    */
    uint totalNbOfItems() const;

public:
    //! Reference to the study
    Data::Study::Ptr study;
    //! Flag to fastly know if the selection is empty
    bool empty;
    //! All selected areas
    Data::Area::Set areas;
    //! All selected links
    Data::AreaLink::Set links;
    //! All selected thermal clusters
    Data::ThermalCluster::Set ThClusters;
    //! All selected renewable clusters
    Data::RenewableCluster::Set RnClusters;
    Data::HydroclusterCluster::Set HydroclusterClusters;

    //! All selected binding constraints
    Data::BindingConstraint::Set constraints;
    //! All studies
    Data::Study::Set studies;

}; // class InspectorData

// Singleton
extern InspectorData::Ptr gData;

} // namespace Inspector
} // namespace Window
} // namespace Antares

#include "data.hxx"

#endif // __ANTARES_WINDOWS_INSPECTOR_DATA_H__
