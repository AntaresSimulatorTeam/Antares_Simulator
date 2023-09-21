/*
** Copyright 2007-2023 RTE
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
#ifndef __ANTARES_LIBS_STUDY_ACTION_CONTEXT_H__
#define __ANTARES_LIBS_STUDY_ACTION_CONTEXT_H__

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include <map>
#include <yuni/core/string.h>
#include <yuni/core/bind.h>
#include <memory>
#include "action.h"
#include <antares/study/binding_constraint/BindingConstraintsRepository.h>

namespace Antares
{
namespace Action
{
class Context
{
public:
    using Ptr = std::shared_ptr<Context>;
    //! Map of properties
    using Properties = std::map<Yuni::String, Yuni::String>;
    //! Local view
    using LocalView = std::map<Yuni::CString<50, false>, IAction::Ptr>;
    using Views = std::map<Yuni::CString<50, false>, LocalView>;

    //! Area name mapping
    using AreaNameMappingForceCreate = std::map<Data::AreaName, bool>;
    using PlantNameMappingForceCreate = std::map<Data::AreaName, bool>;
    using ConstraintMappingForceCreate = std::map<Data::ConstraintName, bool>;

public:
    //! \name Constructor & Destructor
    //@{
    explicit Context(Data::Study::Ptr targetStudy, const size_t layer = 0);
    //! Destructor
    ~Context() = default;
    //@}

    /*!
    ** \brief Reset internal data
    */
    void reset();

public:
    //! The target study
    Data::Study::Ptr study;
    //! An optional external study, which will be destroyed with this class
    Data::Study::Ptr extStudy;

    // The current Layer
    const size_t layerID;

    //! The current Area
    Data::Area* area;
    //! The current Thermal cluster
    Data::ThermalCluster* cluster;
    //! The current Link
    Data::AreaLink* link;
    //! The original cluster name
    Data::ThermalCluster* originalPlant;
    //! The current constraint
    std::shared_ptr<Data::BindingConstraint> constraint;

    //! Views
    Views view;
    //! All properties
    Properties property;

    //! Area name mapping
    Data::Area::NameMapping areaNameMapping;
    Data::Area::NameMapping areaLowerNameMapping;
    //! Area name mapping, force creation
    AreaNameMappingForceCreate areaForceCreate;
    PlantNameMappingForceCreate clusterForceCreate;
    ConstraintMappingForceCreate constraintForceCreate;

    //! Temporary string for message formatting
    Yuni::String message;
    //! Delegate to update the interface
    Yuni::Bind<void(const Yuni::String&)> updateMessageUI;

    //! \name Autoselect
    //@{
    //! List of areas to select after the paste
    Data::Area::Vector autoselectAreas;
    //! List of links to select after the paste
    Data::AreaLink::Vector autoselectLinks;
    //! List of binding constraints to select after the paste
    Data::BindingConstraintsRepository::Vector autoselectConstraints;
    //@}

}; // class Context

} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_CONTEXT_H__
