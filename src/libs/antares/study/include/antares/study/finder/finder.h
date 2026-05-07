// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIB_FINDER_FINDER_H__
#define __ANTARES_LIB_FINDER_FINDER_H__

#include <mutex>

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/io/directory/iterator.h>

#include "antares/study/study.h"

#include "../version.h"

namespace Antares::Data
{
/*!
** \brief Look for study folders asynchronously
*/
class StudyFinder
{
public:
    enum
    {
        //! The default value for the timeout
        defaultTimeout = 10000, // 10s
    };

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
    virtual void onStudyFound(const Yuni::String& folder, const StudyVersion& version) = 0;

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

protected:
    std::mutex mutex;

private:
    Yuni::IO::Directory::IIterator<true>* pLycos;

}; // class StudyFinder

} // namespace Antares::Data

#endif // __ANTARES_LIB_FINDER_FINDER_H__
