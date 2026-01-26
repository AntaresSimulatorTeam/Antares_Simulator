// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_SIMULATION_H__
#define __ANTARES_LIBS_STUDY_SIMULATION_H__

#include <yuni/yuni.h>

#include <antares/writer/i_writer.h>

#include "fwd.h"

namespace Antares::Data
{
/*!
** \brief Set of settings for a simulation
*/

// TODO VP: remove with GUI
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

public:
    //! Comments
    YString comments;
    //! name
    YString name;

private:
    Study& pStudy;

}; // class SimulationComments

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_SIMULATION_H__
