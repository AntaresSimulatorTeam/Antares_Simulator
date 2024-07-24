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
#ifndef __ANTARES_FS_WALKER_FS_WALKER_H__
#define __ANTARES_FS_WALKER_FS_WALKER_H__

#include <memory>

#include <yuni/yuni.h>
#include <yuni/core/bind.h>
#include <yuni/core/noncopyable.h>
#include <yuni/core/string.h>
#include <yuni/thread/thread.h>

#include "job.h"
#include "statistics.h"

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
                             int64_t modified,
                             uint64_t size,
                             void* user);
//! Event for dispatching a new job
using DispatchJobEvent = std::function<void(IJob::Ptr job)>;

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
class Walker final: public Yuni::NonCopyable<Walker>
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
