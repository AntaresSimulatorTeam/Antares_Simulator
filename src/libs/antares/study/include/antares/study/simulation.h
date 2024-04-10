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
#ifndef __ANTARES_LIBS_STUDY_SIMULATION_H__
#define __ANTARES_LIBS_STUDY_SIMULATION_H__

#include <yuni/yuni.h>

#include <antares/writer/i_writer.h>

#include "fwd.h"

namespace Antares
{
namespace Data
{
/*!
** \brief Set of settings for a simulation
*/
class SimulationComments final
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    SimulationComments(Study& study);

    //! Destructor
    ~SimulationComments()
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

    void saveUsingWriter(Solver::IResultWriter& writer, const AnyString& folder) const;

    //! Get (in bytes) the amount of memory used by the class
    uint64_t memoryUsage() const;

public:
    //! Comments
    YString comments;
    //! name
    YString name;

private:
    Study& pStudy;

}; // class SimulationComments

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_SIMULATION_H__
