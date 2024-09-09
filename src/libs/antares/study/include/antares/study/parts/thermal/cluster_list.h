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
#pragma once

#include <functional>

#include "../common/cluster_list.h"
#include "cluster.h"

namespace Antares
{
namespace Data
{
/*!
** \brief List of clusters
** \ingroup thermalclusters
*/
class ThermalClusterList: public ClusterList<ThermalCluster>
{
public:
    std::string typeID() const override;
    uint64_t memoryUsage() const override;

    /*!
    ** \brief Get the size (bytes) occupied in memory by a `ThermalClusterList` structure
    ** \return A size (in bytes)
    */
    bool loadFromFolder(Study& s, const AnyString& folder, Area* area);

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ThermalClusterList();
    /*!
    ** \brief Destructor
    */
    virtual ~ThermalClusterList();
    //@}

    //! \name Spinning
    //@{
    /*!
    ** \brief Calculation of Spinning for all thermal clusters
    */
    void calculationOfSpinning();

    /*!
    ** \brief Calculation of Spinning for all thermal clusters (reverse)
    */
    void reverseCalculationOfSpinning();
    //@}

    //! \name Mustrun
    //@{
    /*!
    ** \brief Enable the 'mustrun' mode for every cluster
    **
    ** This method is especially useful for the adequacy mode, where all clusters
    ** must be in mustrun mode
    */
    void enableMustrunForEveryone();

    //@}

    auto each_mustrun_and_enabled() const
    {
        return allClusters_ | std::views::filter(&ThermalCluster::isMustRun)
               | std::views::filter(&ThermalCluster::isEnabled);
    }

    auto each_enabled_and_not_mustrun() const
    {
        return allClusters_ | std::views::filter(&ThermalCluster::isEnabled)
               | std::views::filter(std::not_fn(&ThermalCluster::isMustRun));
    }

    /*!
    ** \brief Ensure data for the prepro are initialized
    ** \ingroup thermalclusters
    **
    ** \todo Remaining of old C-library. this routine should be moved into the appropriate class
    ** \param l The list of thermal clusters
    */
    void ensureDataPrepro();

    /*!
     ** \brief Load data related to the preprocessor from a list of thermal clusters from a folder
     ** \ingroup thermalclusters
     **
     ** \todo Remaining of old C-library. this routine should be moved into the appropriate class
     ** \param l A list of thermal clusters
     ** \param folder The target folder
     ** \return A non-zero value if the operation succeeded, 0 otherwise
     */
    bool loadPreproFromFolder(Study& s, const AnyString& folder);
    bool validatePrepro(const Study& study);

    bool validateClusters(const Parameters& param) const;

    bool loadEconomicCosts(Study& s, const AnyString& folder);

    bool savePreproToFolder(const AnyString& folder) const;
    bool saveEconomicCosts(const AnyString& folder) const;

    bool saveToFolder(const AnyString& folder) const override;

    unsigned int enabledAndMustRunCount() const;
    unsigned int enabledAndNotMustRunCount() const;

    // returns the number of reserve participations of all clusters
    unsigned int reserveParticipationsCount() const;
    // returns the number of capacity reserves that all the clusters are participating to 
    // (count only capacity resarvations once)
    unsigned int capacityReservationsCount() const;

private:
    // Give a special index to enbled and not must-run THERMAL clusters
    void rebuildIndex() const;

}; // class ThermalClusterList
} // namespace Data
} // namespace Antares
