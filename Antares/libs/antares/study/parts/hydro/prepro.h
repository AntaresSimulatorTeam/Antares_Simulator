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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_PREPRO_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_PREPRO_H__

#include "../../../array/matrix.h"
#include "../../fwd.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Data for the hydro preprocessor
** \ingroup hydroprepro
*/
class PreproHydro
{
public:
    enum
    {
        /*!
        ** \brief The expectation of the total capacity (Espérance du logarithme de 'energie totale)
        **
        ** These values will be modified before launching a simulation.
        ** The solver requires log(expectaction) and log(stddeviation)
        */
        expectation = 0,
        /*!
        ** \brief The standard deviation of the total capacity (ecart-type du logarithme de
        *l'energie totale)
        **
        ** These values will be modified before launching a simulation.
        ** The solver requires log(expectaction) and log(stddeviation)
        */
        stdDeviation,
        //! Minimum capacity per month
        minimumEnergy,
        //! Maximum capacity per month
        maximumEnergy,
        /*!
        ** \brief Power over water
        **
        ** Proportion du fil de l'eau : l'hydraulique est representee au travers de son
        ** energie totale mensuelle; cette proportion est donc une proportion en énergie
        ** La valeur peut varier entre 0 (pas de fil de l'eau) et 1 (pas d'hydraulique modulable)
        */
        powerOverWater,
        //! The maximum number of item
        hydroPreproMax

    }; // anonymous enum

public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    PreproHydro();
    //@}

    void estimateMemoryUsage(StudyMemoryUsage&) const;

    /*!
    ** \brief Reset all data to their default values
    */
    void reset();

    /*!
    ** \brief Copy data from another PreproHydro structure
    */
    void copyFrom(const PreproHydro& rhs);

    /*!
    ** \brief Load hydro settings for the prepro from a folder
    **
    ** \param folder The source folder (ex: `input/hydro/prepro`)
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool loadFromFolder(Study& s, const AreaName& areaID, const char folder[]);

    /*!
    ** \brief Save hydro settings for the prepro into a folder
    **
    ** \param folder The target folder (ex: `input/hydro/prepro`)
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool saveToFolder(const AreaName& areaID, const char* folder);

    bool forceReload(bool reload = false) const;

    void markAsModified() const;

public:
    //! The intermonthly correlation value
    double intermonthlyCorrelation;
    //! data
    Matrix<> data;

}; // class PreproHydro

int PreproHydroLoadFromFolder(Study& s,
                              PreproHydro* h,
                              const AreaName& areaID,
                              const char folder[]);

/*!
** \brief Get the size (bytes) occupied in memory by a `PreproHydro` structure
** \ingroup hydroprepro
*/
Yuni::uint64 PreproHydroMemoryUsage(PreproHydro* h);

} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_PREPRO_H__ */
