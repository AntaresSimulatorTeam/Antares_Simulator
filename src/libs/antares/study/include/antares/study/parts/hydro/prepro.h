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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_PREPRO_H__
#define __ANTARES_LIBS_STUDY_PARTS_HYDRO_PREPRO_H__

#include <antares/array/matrix.h>

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
    bool loadFromFolder(Study& s, const AreaName& areaID, const std::string& folder);

    bool validate(const std::string& areaID);
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

} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_PREPRO_H__ */
