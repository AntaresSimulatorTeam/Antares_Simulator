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
#ifndef __ANTARES_LIB_FINDER_FINDER_H__
#define __ANTARES_LIB_FINDER_FINDER_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "../study.h"
#include <yuni/io/directory/iterator.h>

namespace Antares
{
namespace Data
{
/*!
** \brief Look for study folders asynchronously
*/
class StudyFinder : public Yuni::Policy::ObjectLevelLockable<StudyFinder>
{
public:
    //! The threading policy
    typedef Yuni::Policy::ObjectLevelLockable<StudyFinder> ThreadingPolicy;
    enum
    {
        //! The default value for the timeout
        defaultTimeout = 10000, // 10s
    };

    //! List of folders
    typedef std::vector<Yuni::String::Ptr> FolderVector;

public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    StudyFinder();
    //! Copy constructor
    StudyFinder(const StudyFinder&);
    /*!
    ** \brief Destructor
    */
    virtual ~StudyFinder();
    //@}

    /*!
    ** \brief Start a new lookup from a single folder
    **
    ** If a lookup was already in progress, it will be stopped before.
    ** \param folder The root folder where to start the lookup
    */
    void lookup(const Yuni::String& folder);

    /*!
    ** \brief Start a new lookup from a list of folder
    **
    ** If a lookup was already in progress, it will be stopped before.
    ** \param folders The list of folders where to start the lookup
    */
    void lookup(const Yuni::String::Vector& folders);
    /*!
    ** \brief Start a new lookup from a list of folder
    **
    ** If a lookup was already in progress, it will be stopped before.
    ** \param folders The list of folders where to start the lookup
    */
    void lookup(const Yuni::String::List& folders);

    /*!
    ** \brief Stop a lookup currently in progress
    */
    void stop(uint timeout = defaultTimeout);

    /*!
    ** \brief Wait Indefinitely for the end of the lookup
    */
    void wait();

    /*!
    ** \brief Wait for the end of the lookup (with timeout)
    */
    void wait(uint timeout);

public: // Events
    /*!
    ** \brief Event triggered when a lookup has been found
    */
    virtual void onStudyFound(const Yuni::String& folder, Version version) = 0;

    /*!
    ** \brief Event triggered when a lookup has finished gracefully
    **
    ** This event might be called from any thread.
    */
    virtual void onLookupFinished()
    {
    }

    /*!
    ** \brief Event triggered when a lookup has been aborted
    **
    ** This event might be called from any thread.
    */
    virtual void onLookupAborted()
    {
    }

private:
    Yuni::IO::Directory::IIterator<true>* pLycos;

}; // class StudyFinder

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIB_FINDER_FINDER_H__
