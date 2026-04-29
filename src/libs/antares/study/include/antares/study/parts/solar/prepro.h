// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_SOLAR_PREPRO_H__
#define __ANTARES_LIBS_STUDY_PARTS_SOLAR_PREPRO_H__

#include "../../xcast.h"

namespace Antares::Data::Solar
{
/*!
** \brief Prepro Solar
*/
class Prepro final
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
    ** \brief Solar data from a folder
    */
    bool loadFromFolder(const std::filesystem::path& folder);

    /*!
    ** \brief Save data to a folder
    */
    bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Reset to default all values
    */
    void resetToDefault();

public:
    //! XCast
    Data::XCast xcast;

private:
    bool internalSolarFromFolder(Study& study, const char* folder, uint length);
    bool internalSolarFormatBefore33(Study& study, const YString& folder);

}; // Prepro

} // namespace Antares::Data::Solar

#include "prepro.hxx"

#endif // __ANTARES_LIBS_STUDY_PARTS_SOLAR_PREPRO_H__
