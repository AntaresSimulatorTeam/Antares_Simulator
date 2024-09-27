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
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_PREPRO_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_PREPRO_H__

#include <memory>

#include <antares/array/matrix.h>
#include <antares/solver/ts-generator/law.h>
#include <antares/study/fwd.h>
#include <antares/study/parts/thermal/defines.h>

namespace Antares::Data
{
/*!
** \brief Thermal
*/
class PreproAvailability
{
public:
    enum
    {
        //! FO Duration (Forced outage Duration - Duree moyenne d'indisponibilite fortuite)
        foDuration = 0,
        //! PO Duration (Planned outage Duration - Duree moyenne d'indisponibilite programmee)
        poDuration,
        //! FO Rate (Forced outage Rate - Taux moyen d'indisponibilite fortuite)
        foRate,
        //! PO Rate (Planned outage Rate - Taux moyen d'indisponibilite programmee)
        poRate,
        //! NPO min (nombre minimal de groupes en maintenance)
        npoMin,
        //! NPO max (nombre maximal de groupes en maintenance)
        npoMax,
        // max
        preproAvailabilityMax,
    };

    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit PreproAvailability(const YString& id, unsigned int unitCount);
    //@}

    bool forceReload(bool reload) const;

    void markAsModified() const;

    /*!
    ** \brief Reset all values to their default ones
    */
    void reset();

    //! Copy data from another struct
    void copyFrom(const PreproAvailability& rhs);

    /*!
    ** \brief Load settings for the thermal prepro from a folder
    **
    ** \param folder The source folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool loadFromFolder(Study& study, const std::filesystem::path& folder);

    /*!
    ** \brief Validate most settings against min/max rules
    */
    bool validate() const;

    /*!
    ** \brief Save settings used by the thermal prepro to a folder
    **
    ** \param folder The targer folder
    ** \return A non-zero value if the operation succeeded, 0 otherwise
    */
    bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Get the amount of memory used by the class
    */
    uint64_t memoryUsage() const;

    /*!
    ** \brief Normalize NPO max and check for consistency
    **
    ** This method should only be used by the solver
    */
    bool normalizeAndCheckNPO();

    //! All {FO,PO}{Duration,Rate} annual values
    // max x DAYS_PER_YEAR
    Matrix<> data;
    // Parent thermal cluster
    YString id;
    unsigned int unitCount;
}; // class PreproAvailability
} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_PARTS_THERMAL_PREPRO_HXX__
