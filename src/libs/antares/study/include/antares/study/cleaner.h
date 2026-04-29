// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_CLEANER_H__
#define __ANTARES_LIBS_STUDY_CLEANER_H__

#include <yuni/yuni.h>
#include <yuni/core/bind.h>
#include <yuni/core/string.h>

#include <antares/paths/list.h>

#include "version.h"

namespace Antares::Data
{
class StudyCleaningInfos final
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    StudyCleaningInfos();
    /*!
    ** \brief Constructor with a given path
    */
    explicit StudyCleaningInfos(const AnyString& path);
    /*!
    ** \brief Destructor
    */
    ~StudyCleaningInfos();
    //@}

    /*!
    ** \brief Determine the complete list of all files/folder considered as intruders
    */
    bool analyze();

    /*!
    ** \brief Perform a complete cleanup
    */
    void performCleanup();

    /*!
    ** \brief Get the total size occupied by the intruders
    */
    uint64_t totalSize() const;

    /*!
    ** \brief Set an additional list of excluded files or directories.
    */
    void setCustomExcludeList(const Yuni::String& c);

public:
    /*! A study folder */
    Yuni::String folder;

    /*! Version of the study */
    StudyVersion version;
    /*! List of all intruders */
    ::PathList intruders;
    /*! List of all files/folders to prevent from the destruction */
    ::PathList exclude;
    /*! Colon-separated list of all files/folders to prevent from the destruction */
    Yuni::String customExclude;

    /*! List of all files/folders to exclude after analyzing the content of the folder */
    /* This list mainly ensures that a list of folders will not deleted enven if empty */
    ::PathList postExclude;

    /*!
    ** \brief Event triggered when entering into a new folder
    **
    ** Return false to stop the process
    */
    std::function<bool(uint)> onProgress;

}; // class StudyCleaningInfos

} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_CLEANER_H__ */
