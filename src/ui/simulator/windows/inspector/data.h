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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_WINDOWS_INSPECTOR_DATA_H__
#define __ANTARES_WINDOWS_INSPECTOR_DATA_H__

#include <yuni/yuni.h>
#include <memory>
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"
#include <antares/study/study.h>

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
    //! All selected binding constraints
    Data::BindingConstraintsRepository::Set constraints;
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
