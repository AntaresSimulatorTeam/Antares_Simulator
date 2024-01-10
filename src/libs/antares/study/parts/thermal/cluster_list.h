/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include "../common/cluster_list.h"

namespace Antares
{
namespace Data
{
/*!
** \brief List of clusters
** \ingroup thermalclusters
*/
class ThermalClusterList : public ClusterList<ThermalCluster>
{
public:
    std::string typeID() const override;
    // Map container
    using Map = typename std::map<ClusterName, std::shared_ptr<ThermalCluster>>;

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

    Map mapping;

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
    bool loadPreproFromFolder(Study& s, const StudyLoadOptions& options, const AnyString& folder);

    bool loadEconomicCosts(Study& s, const AnyString& folder);

    bool savePreproToFolder(const AnyString& folder) const;
    bool saveEconomicCosts(const AnyString& folder) const;

    bool saveToFolder(const AnyString& folder) const override;
}; // class ThermalClusterList
} // namespace Data
} // namespace Antares
