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
#ifndef __ANTARES_LIBS_STUDY_HEADER_H__
#define __ANTARES_LIBS_STUDY_HEADER_H__

#include <ctime>
#include <filesystem>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/inifile/inifile.h>

#include "version.h"

//! Default author
#define STUDYHEADER_DEFAULT_AUTHOR "Unknown"
//! Default caption
#define STUDYHEADER_DEFAULT_CAPTION "No title"

namespace Antares
{
namespace Data
{
/*!
** \brief Header of a study
** \ingroup study
*/
class StudyHeader final
{
public:
    /*!
    ** \brief Get the version of a header
    **
    ** \param filename The filename to read
    ** \return The version of the study, or 0 if unknown (invalid header)
    */
    static bool readVersionFromFile(const std::filesystem::path& filename, std::string& version);

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    StudyHeader()
    {
        reset();
    }

    /*!
    ** \brief Destructor
    */
    ~StudyHeader() = default;
    //@}

    /*!
    ** \brief Reset the values
    */
    void reset();

    /*!
    ** \brief Load a study header from a file
    **
    ** \param filename The target filename
    ** \param warnings Enable warnings/errors
    ** \return True if the operation succeeded, false otherwise
    */
    bool loadFromFile(const std::filesystem::path& filename, bool warnings = true);

    /*!
    ** \brief Save a study header into a file
    **
    ** The property `version` will be upgraded to the last available
    ** and `dateLastSave` as well.
    **
    ** \param filename The target filename
    ** \return True if the operation succeeded, false otherwise
    */
    bool saveToFile(const AnyString& filename, bool upgradeVersion = true);

    //! Copy the internal settings into an INI structure
    void CopySettingsToIni(IniFile& ini, bool upgradeVersion);

    /*!
     ** \brief Try to determine the version of a study
     ** \ingroup study
     **
     ** \param folder The folder where data are located
     ** \return The version of the study. `unknown` if not found
     */
    static StudyVersion tryToFindTheVersion(const std::string& folder);

    bool validateVersion();

    //! Caption of the study
    Yuni::String caption;

    //! Format version
    StudyVersion version;

    //! Date: Creation (timestamp)
    time_t dateCreated;
    //! Date: Last save (timestamp)
    time_t dateLastSave;

    //! Author
    Yuni::String author;

private:
    //! Load settings from an INI file
    bool internalLoadFromINIFile(const IniFile& ini, bool warnings);

    //! Get the version written in an header file
    static bool internalFindVersionFromFile(const IniFile& ini, std::string& version);

}; // class StudyHeader;

} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_HEADER_H__ */
