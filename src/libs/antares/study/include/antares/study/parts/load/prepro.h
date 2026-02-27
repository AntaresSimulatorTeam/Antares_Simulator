// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "../../fwd.h"
#include "../../xcast.h"

namespace Antares::Data::Load
{
/*!
** \brief Prepro Load
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
    ** \brief Load data from a folder
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
    bool internalLoadFromFolder(Study& study, const char* folder, uint length);

}; // Prepro

} // namespace Antares::Data::Load

#include "prepro.hxx"
