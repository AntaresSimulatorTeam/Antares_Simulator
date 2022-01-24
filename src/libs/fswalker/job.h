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
#ifndef __ANTARES_FS_WALKER_JOB_H__
#define __ANTARES_FS_WALKER_JOB_H__

#include <yuni/yuni.h>
#include <yuni/job/job.h>

#include <memory>

namespace FSWalker
{
// Forward declaration
class Walker;
class WalkerThread;

//! Internal job counter
typedef std::shared_ptr<Yuni::Atomic::Int<32>> InternalJobCounter;

class IJob : public Yuni::Job::IJob
{
public:
    //! The most suitable smart pointer for the class
    // typedef std::shared_ptr<IJob> Ptr;
    typedef Yuni::Job::IJob::Ptr::Promote<IJob>::Ptr Ptr;

public:
    IJob()
    {
    }
    virtual ~IJob();

protected:
    void decrementCounter();

protected:
    InternalJobCounter pJobCounter;
    friend class WalkerThread;
};

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_JOB_H__
