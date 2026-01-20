// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/finder/finder.h"

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/io/directory/iterator.h>

#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data
{
namespace // anonymous namespace
{
class MyIterator final: public IO::Directory::IIterator<true>
{
public:
    using IteratorType = IO::Directory::IIterator<true>;
    using Flow = IO::Flow;

public:
    MyIterator(StudyFinder& finder):
        pFinder(finder)
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
        const StudyVersion versionFound = StudyHeader::tryToFindTheVersion(filename);
        if (versionFound > StudyVersion::latest())
        {
            return IO::flowSkip;
        }
        if (versionFound == StudyVersion::unknown())
        {
            return IO::flowContinue;
        }

        // We have found a study !
        pFinder.onStudyFound(filename, versionFound);
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

StudyFinder::StudyFinder():
    pLycos(nullptr)
{
}

StudyFinder::StudyFinder(const StudyFinder&):
    pLycos(nullptr)
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
    std::lock_guard locker(mutex);
    if (pLycos)
    {
        pLycos->stop(timeout);
    }
}

void StudyFinder::wait()
{
    std::lock_guard locker(mutex);
    if (pLycos)
    {
        pLycos->wait();
    }
}

void StudyFinder::wait(uint timeout)
{
    std::lock_guard locker(mutex);
    if (pLycos)
    {
        pLycos->wait(timeout);
    }
}

void StudyFinder::lookup(const Yuni::String::Vector& folder)
{
    std::lock_guard locker(mutex);
    if (pLycos)
    {
        pLycos->stop(10000);
    }
    else
    {
        pLycos = new MyIterator(*this);
    }

    pLycos->clear();
    const Yuni::String::Vector::const_iterator end = folder.end();
    for (Yuni::String::Vector::const_iterator i = folder.begin(); i != end; ++i)
    {
        pLycos->add(*i);
    }

    pLycos->start();
}

void StudyFinder::lookup(const Yuni::String::List& folder)
{
    std::lock_guard locker(mutex);
    if (pLycos)
    {
        pLycos->stop(10000);
    }
    else
    {
        pLycos = new MyIterator(*this);
    }

    pLycos->clear();
    const Yuni::String::List::const_iterator end = folder.end();
    for (Yuni::String::List::const_iterator i = folder.begin(); i != end; ++i)
    {
        pLycos->add(*i);
    }
    pLycos->start();
}

void StudyFinder::lookup(const String& folder)
{
    std::lock_guard locker(mutex);
    if (pLycos)
    {
        pLycos->stop(10000);
    }
    else
    {
        pLycos = new MyIterator(*this);
    }

    pLycos->clear();
    pLycos->add(folder);
    pLycos->start();
}

} // namespace Antares::Data
