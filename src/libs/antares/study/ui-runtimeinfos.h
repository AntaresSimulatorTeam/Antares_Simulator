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
#ifndef __ANTARES_LIBS_STUDY_UI_RUNTIME_INFOS_H__
#define __ANTARES_LIBS_STUDY_UI_RUNTIME_INFOS_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "fwd.h"
#include "area/area.h"
#include "binding_constraint/BindingConstraint.h"

namespace Antares
{
namespace Data
{
class UIRuntimeInfo final
{
public:
    using VectorByType
      = std::map<enum BindingConstraint::Type, BindingConstraintsRepository::Vector>;
    using ByOperatorAndType = std::map<enum BindingConstraint::Operator, VectorByType>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    UIRuntimeInfo(Study& study);
    //! Destructor
    ~UIRuntimeInfo()
    {
    }
    //@}

    /*!
    ** \brief Reload all informations about the study
    */
    void reloadAll();

    /*!
    ** \brief Reload informations about the study
    */
    void reload();

    void reloadBindingConstraints();

    /*!
    ** \brief The total number of links in the study
    */
    uint linkCount() const
    {
        return pLinkCount;
    }

    /*!
    ** \brief The total number of links in the clusters
    */
    uint clusterCount() const
    {
        return pClusterCount;
    }

    /*!
    ** \brief Get the link according a given index
    */
    AreaLink* link(uint i)
    {
        assert(i < pLink.size());
        return pLink[i];
    }
    const AreaLink* link(uint i) const
    {
        assert(i < pLink.size());
        return pLink[i];
    }

    /*!
    ** \brief Get the cluster according a given index
    */
    ThermalCluster* cluster(uint i)
    {
        assert(i < pClusters.size());
        return pClusters[i];
    }
    const ThermalCluster* cluster(uint i) const
    {
        assert(i < pClusters.size());
        return pClusters[i];
    }

    BindingConstraint* constraint(uint i)
    {
        assert(i < pConstraint.size());
        return pConstraint[i].get();
    }
    const BindingConstraint* constraint(uint i) const
    {
        assert(i < pConstraint.size());
        return pConstraint[i].get();
    }
    uint constraintCount() const
    {
        return (uint)pConstraint.size();
    }

    uint countItems(BindingConstraint::Operator op, BindingConstraint::Type type);

    uint visibleClustersCount(uint layerID);

    uint visibleLinksCount(uint layerID);

    uint64_t memoryUsage() const;

public:
    //! Areas ordered by their name + links ordered by their name
    Area::LinkMap orderedAreasAndLinks;
    //! Binding constraints ordered by their name
    BindingConstraintsRepository::Set orderedConstraint;
    //! All binding constraints according their operator (<, > and = only)
    ByOperatorAndType byOperator;
    //! Vector of pointers to links, in lexicographic order
    AreaLink::Vector pLink;

private:
    Study& pStudy;
    BindingConstraintsRepository::Vector pConstraint;
    uint pClusterCount;
    ThermalCluster::Vector pClusters;
    //! Number of links
    uint pLinkCount;
}; // class UIRuntimeInfo

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_UI_RUNTIME_INFOS_H__
