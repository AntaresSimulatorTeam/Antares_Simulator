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

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/io/directory/iterator.h>
#include "../study.h"
#include "finder.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace // anonymous namespace
{
class MyIterator final : public IO::Directory::IIterator<true>
{
public:
    typedef IO::Directory::IIterator<true> IteratorType;
    typedef IO::Flow Flow;

public:
    MyIterator(StudyFinder& finder) : pFinder(finder)
    {
    }
    virtual ~MyIterator()
    {
        // For code robustness and to avoid corrupt vtable
        stop();
    }

protected:
    virtual bool onStart(const String& filename)
    {
        return (IO::flowContinue == onBeginFolder(filename, filename, filename));
    }

    virtual Flow onBeginFolder(const String& filename, const String&, const String&)
    {
        const Version versionFound = StudyTryToFindTheVersion(filename);
        switch (versionFound)
        {
        case version1xx: // skipped
            return IO::flowSkip;
        case versionFutur:
            return IO::flowSkip;
        case versionUnknown:
            return IO::flowContinue;
        default:
        {
            // We have found a study !
            pFinder.onStudyFound(filename, versionFound);
            return IO::flowSkip;
        }
        }
        return IO::flowSkip;
    }

    virtual void onTerminate()
    {
        pFinder.onLookupFinished();
    }

    virtual void onAbort()
    {
        pFinder.onLookupAborted();
    }

public:
    StudyFinder& pFinder;
};

} // anonymous namespace

StudyFinder::StudyFinder() : pLycos(nullptr)
{
}

StudyFinder::StudyFinder(const StudyFinder&) : ThreadingPolicy(), pLycos(nullptr)
{
}

StudyFinder::~StudyFinder()
{
    if (pLycos)
    {
        stop();
        delete pLycos;
    }
}

void StudyFinder::stop(uint timeout)
{
    ThreadingPolicy::MutexLocker locker(*this);
    if (pLycos)
        pLycos->stop(timeout);
}

void StudyFinder::wait()
{
    ThreadingPolicy::MutexLocker locker(*this);
    if (pLycos)
        pLycos->wait();
}

void StudyFinder::wait(uint timeout)
{
    ThreadingPolicy::MutexLocker locker(*this);
    if (pLycos)
        pLycos->wait(timeout);
}

void StudyFinder::lookup(const Yuni::String::Vector& folder)
{
    ThreadingPolicy::MutexLocker locker(*this);
    if (pLycos)
        pLycos->stop(10000);
    else
        pLycos = new MyIterator(*this);

    pLycos->clear();
    const Yuni::String::Vector::const_iterator end = folder.end();
    for (Yuni::String::Vector::const_iterator i = folder.begin(); i != end; ++i)
        pLycos->add(*i);

    pLycos->start();
}

void StudyFinder::lookup(const Yuni::String::List& folder)
{
    ThreadingPolicy::MutexLocker locker(*this);
    if (pLycos)
        pLycos->stop(10000);
    else
        pLycos = new MyIterator(*this);

    pLycos->clear();
    const Yuni::String::List::const_iterator end = folder.end();
    for (Yuni::String::List::const_iterator i = folder.begin(); i != end; ++i)
        pLycos->add(*i);
    pLycos->start();
}

void StudyFinder::lookup(const String& folder)
{
    ThreadingPolicy::MutexLocker locker(*this);
    if (pLycos)
        pLycos->stop(10000);
    else
        pLycos = new MyIterator(*this);

    pLycos->clear();
    pLycos->add(folder);
    pLycos->start();
}

} // namespace Data
} // namespace Antares
