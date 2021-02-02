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
#ifndef __ANTARES_LIBS_STUDY_PARTS_WIND_PREPRO_H__
#define __ANTARES_LIBS_STUDY_PARTS_WIND_PREPRO_H__

#include "../../fwd.h"
#include "../../xcast.h"

namespace Antares
{
namespace Data
{
namespace Wind
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
    void estimateMemoryUsage(StudyMemoryUsage&) const;

    /*!
    ** \brief Load data from a folder
    */
    bool loadFromFolder(Study& study, const AnyString& folder);

    /*!
    ** \brief Save data to a folder
    */
    bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Get the amount of memory currently used
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Make sure that all data are loaded in memory
    */
    bool invalidate(bool reload = false) const;

    void markAsModified() const;

    /*!
    ** \brief Reset to default all values
    */
    void resetToDefault();

public:
    //! XCast
    Data::XCast xcast;

private:
    bool internalLoadFormatBefore33(Study& study, const AnyString& folder);

}; // Prepro

} // namespace Wind
} // namespace Data
} // namespace Antares

#include "prepro.hxx"

#endif // __ANTARES_LIBS_STUDY_PARTS_WIND_PREPRO_H__
