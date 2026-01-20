// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

namespace Antares::Data
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
    Yuni::String editor;

private:
    //! Load settings from an INI file
    bool internalLoadFromINIFile(const IniFile& ini, bool warnings);

    //! Get the version written in an header file
    static bool internalFindVersionFromFile(const IniFile& ini, std::string& version);

}; // class StudyHeader;

} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_HEADER_H__ */
