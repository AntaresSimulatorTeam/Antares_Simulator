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
#ifndef __ANTARES_LIBS_STUDY_PARTS_WIND_PREPRO_H__
#define __ANTARES_LIBS_STUDY_PARTS_WIND_PREPRO_H__

#include "../../fwd.h"
#include "../../xcast.h"

namespace Antares::Data::Wind
{
/*!
** \brief Prepro Wind
*/
class Prepro
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Prepro();
    //! Destructor
    ~Prepro();
    //@}

    /*!
    ** \brief Try to estimate the amount of memory required by this class for a simulation
    */

    /*!
    ** \brief Load data from a folder
    */
    bool loadFromFolder(const std::filesystem::path& folder);

    /*!
    ** \brief Save data to a folder
    */
    bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Get the amount of memory currently used
    */
    uint64_t memoryUsage() const;

    /*!
    ** \brief Make sure that all data are loaded in memory
    */
    bool forceReload(bool reload = false) const;

    void markAsModified() const;

    /*!
    ** \brief Reset to default all values
    */
    void resetToDefault();

    //! XCast
    Data::XCast xcast;

}; // Prepro

} // namespace Antares::Data::Wind

#include "prepro.hxx"

#endif // __ANTARES_LIBS_STUDY_PARTS_WIND_PREPRO_H__
