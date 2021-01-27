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
#ifndef __ANTARES_LIBS_STUDY_CLEANER_H__
#define __ANTARES_LIBS_STUDY_CLEANER_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/bind.h>
#include "version.h"
#include "../paths/list.h"

namespace Antares
{
namespace Data
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
    Yuni::uint64 totalSize() const;

public:
    /*! A study folder */
    Yuni::String folder;

    /*! Version of the study */
    Version version;
    /*! List of all intruders */
    ::PathList intruders;
    /*! List of all files/folders to prevent from the destruction */
    ::PathList exclude;
    /*! List of all files/folders to exclude after analyzing the content of the folder */
    /* This list mainly ensures that a list of folders will not deleted enven if empty */
    ::PathList postExclude;

    /*!
    ** \brief Event triggered when entering into a new folder
    **
    ** Return false to stop the process
    */
    Yuni::Bind<bool(uint)> onProgress;

}; // class StudyCleaningInfos

} // namespace Data
} // namespace Antares

#endif /* __ANTARES_LIBS_STUDY_CLEANER_H__ */
