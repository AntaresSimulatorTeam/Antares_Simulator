// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
