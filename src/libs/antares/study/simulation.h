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
#ifndef __ANTARES_LIBS_STUDY_SIMULATION_H__
#define __ANTARES_LIBS_STUDY_SIMULATION_H__

#include <yuni/yuni.h>
#include <i_writer.h>
#include "fwd.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Set of settings for a simulation
*/
class Simulation final
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Simulation(Study& study);
    //! Destructor
    ~Simulation()
    {
    }
    //@}

    /*!
    ** \brief Load a simulation from a folder
    **
    ** \return A non-zero value if succeeded, 0 otherwise
    */
    bool loadFromFolder(const StudyLoadOptions& options);

    /*!
    ** \brief Save settings to the appropriate folder
    */
    bool saveToFolder(const AnyString& folder) const;

    void saveUsingWriter(Solver::IResultWriter::Ptr writer, const AnyString& folder) const;

    //! Get (in bytes) the amount of memory used by the class
    Yuni::uint64 memoryUsage() const;

public:
    //! Comments
    YString comments;
    //! name
    YString name;

private:
    Study& pStudy;

}; // class Simulation

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_SIMULATION_H__
