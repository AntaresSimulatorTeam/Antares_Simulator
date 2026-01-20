// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_UI_RUNTIME_INFOS_H__
#define __ANTARES_LIBS_STUDY_UI_RUNTIME_INFOS_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

#include "area/area.h"
#include "binding_constraint/BindingConstraint.h"
#include "fwd.h"

namespace Antares::Data
{
class UIRuntimeInfo final
{
public:
    using VectorByType = std::map<enum BindingConstraint::Type,
                                  BindingConstraintsRepository::Vector>;
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

public:
    //! Areas ordered by their name + links ordered by their name
    Area::LinkMap orderedAreasAndLinks;
    //! Binding constraints ordered by their name
    BindingConstraint::Set orderedConstraint;
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

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_UI_RUNTIME_INFOS_H__
