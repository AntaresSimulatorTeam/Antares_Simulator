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
#ifndef __ANTARES_LIBS_STUDY_OUTPUT_H__
#define __ANTARES_LIBS_STUDY_OUTPUT_H__

#include <functional>
#include <map>
#include <memory>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include "antares/study/study.h"

namespace Antares
{
namespace Data
{

class Output final
{
public:
    //! Smartptr
    using Ptr = std::shared_ptr<Output>;
    //! List
    using List = std::list<Ptr>;
    //! Title
    using Title = Yuni::CString<128, false>;
    //! Name
    using Name = Yuni::CString<128, false>;

    //! Map of study output info, ordered by their timestamp
    using MapByTimestamp = std::map<int64_t, Ptr>;
    //! Map of study output info, ordered by their timestamp (desc)
    using MapByTimestampDesc = std::map<int64_t, Ptr, std::greater<int64_t>>;

public:
    /*!
    ** \brief Retrieve the list of all available outputs
    */
    static void RetrieveListFromStudy(List& out, const Study& study);

public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Constructor, with an output folder
    */
    explicit Output(const AnyString& folder);
    //@}

    /*!
    ** \brief Load data from an output folder
    */
    bool loadFromFolder(const AnyString& folder);

    //! Get if the output folder previously loaded is valid
    bool valid() const;

    //! Operator <
    bool operator<(const Output& rhs) const;

public:
    //! Version of the solver used for the simulation
    StudyVersion version;
    //! Date/time when the simulation has been launched
    int64_t timestamp;
    //! Mode (economy/adequacy/other)
    Data::SimulationMode mode;
    //! Tag for storing a wx menu id
    int menuID;
    //! Tag for storing a wx menu id (from the menu `output`)
    int viewMenuID;
    //! Tag for storing a wx menu id (for the output viewer)
    int outputViewerID;

    //! Title to use for visual representation of the output
    Title title;
    //! Name
    Name name;
    //! Output folder
    YString path;

}; // class Output

} // namespace Data
} // namespace Antares

#include "output.hxx"

#endif // __ANTARES_STUDY_LIBS_OUTPUT_H__
