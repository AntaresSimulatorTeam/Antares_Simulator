// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_INSPECTOR_DATA_H__
#define __ANTARES_WINDOWS_INSPECTOR_DATA_H__

#include <yuni/yuni.h>
#include <memory>
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"
#include <antares/study/study.h>

namespace Antares::Window::Inspector
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
    Data::BindingConstraint::Set constraints;
    //! All studies
    Data::Study::Set studies;

}; // class InspectorData

// Singleton
extern InspectorData::Ptr gData;

} // namespace Antares::Window::Inspector

#include "data.hxx"

#endif // __ANTARES_WINDOWS_INSPECTOR_DATA_H__
