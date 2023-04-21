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
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_ECOINPUT_H__

#include "cluster.h"
#include "../../../array/matrix.h"
#include "defines.h"
#include "../../fwd.h"
#include <memory>

namespace Antares
{
namespace Data
{
/*!
** \brief Thermal
*/
class EconomicInputData
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit EconomicInputData(std::weak_ptr<const ThermalCluster> cluster);
    //@}

    bool forceReload(bool reload) const;

    void markAsModified() const;

    void estimateMemoryUsage(StudyMemoryUsage&) const;

    /*!
    ** \brief Reset all values to their default ones
    */
    void reset();

    //! Copy data from another struct
    void copyFrom(const EconomicInputData& rhs);

    /*!
    ** \brief Load settings for the thermal prepro from a folder
    **
    ** \param folder The source folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool loadFromFolder(Study& study, const AnyString& folder);

    /*!
    ** \brief Save settings used by the thermal prepro to a folder
    **
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool saveToFolder(const AnyString& folder);

    /*!
    ** \brief Get the amount of memory used by the class
    */
    Yuni::uint64 memoryUsage() const;

    //! All {FO,PO}{Duration,Rate} annual values
    // max x DAYS_PER_YEAR
    Matrix<double> fuelcost;
    Matrix<double> co2cost;
    // Parent thermal cluster
    std::weak_ptr<const ThermalCluster> itsThermalCluster;
}; // class PreproThermal

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_PARTS_THERMAL_PREPRO_HXX__
