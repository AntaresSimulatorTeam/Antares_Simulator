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
#ifndef __ANTARES_FS_WALKER_FS_WALKER_H__
#define __ANTARES_FS_WALKER_FS_WALKER_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/thread/thread.h>
#include <yuni/core/bind.h>
#include <yuni/core/noncopyable.h>
#include "statistics.h"
#include "job.h"

#include <memory>

namespace FSWalker
{
enum Flow
{
    flContinue,
    flAbort,
    flSkip
};

//! Event: Entering into a new folder
using OnDirectoryEvent = Flow (*)(const YString& path, bool empty, void* user);
//! A file has been found
using OnFileEvent = void (*)(const YString& filename,
                             const YString& parent,
                             yint64 modified,
                             yuint64 size,
                             void* user);
//! Event for dispatching a new job
using DispatchJobEvent = Yuni::Bind<void(IJob::Ptr job)>;

class IExtension
{
public:
    //! Most suitable smart pointer
    using Ptr = std::shared_ptr<IExtension>;
    //! List
    using Vector = std::vector<Ptr>;

public:
    /*!
    ** \brief Default constructor
    */
    IExtension()
    {
    }
    /*!
    ** \brief Destructor
    */
    virtual ~IExtension()
    {
    }

    /*!
    ** \brief Human readable caption of the extension
    */
    virtual const char* caption() const = 0;

    /*!
    ** \brief Recommended priority for the extension
    */
    virtual int priority() const
    {
        return 0;
    }

    /*!
    ** \brief Provide a reentrant event for handling directories
    */
    virtual OnDirectoryEvent directoryEvent()
    {
        return nullptr;
    }

    /*!
    ** \brief Provide a reentrant event for handling files
    */
    virtual OnFileEvent fileEvent()
    {
        return nullptr;
    }

    /*!
    ** \brief Get the user data
    */
    virtual void* userdataCreate(DispatchJobEvent&)
    {
        return nullptr;
    }

    /*!
    ** \brief Destroy user data
    */
    virtual void userdataDestroy(void* /*userdata*/)
    {
    }

}; // class IExtension

/*!
** This class is thread-safe.
*/
class Walker final : public Yuni::NonCopyable<Walker>
{
public:
    Walker();
    explicit Walker(const AnyString& logprefix);
    ~Walker();

    void add(IExtension::Ptr extension);

    void directory(const AnyString& path);

    YString directory() const;

    void run();

    /*!
    ** \brief Retrieve statistics for the last run
    */
    void retrieveStatistics(Statistics& out);

private:
    //! Directories to analyze
    YString pDirectory;
    //! Statistics of the last run
    Statistics pStats;
    //! Number of simultaneous jobs
    uint pJobCount;
    //! Extensions
    IExtension::Vector pExtensions;
    //! Log prefix
    YString pLogPrefix;

}; // class Walker

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_FS_WALKER_H__
