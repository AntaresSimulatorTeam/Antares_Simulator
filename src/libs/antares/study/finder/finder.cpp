// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/finder/finder.h"

#include <filesystem>

#include "antares/study/study.h"

namespace Antares::Data
{
namespace // anonymous namespace
{
class Traverser
{
public:
    Traverser(StudyFinder& finder):
        pFinder(finder)
    {
    }

    // Recursively look for study folders from a set of root folders.
    // The traversal is aborted as soon as the `stop` flag is set.
    void run(const std::vector<std::string>& roots, const std::atomic<bool>& stop)
    {
        for (const auto& root: roots)
        {
            if (stop.load())
            {
                pFinder.onLookupAborted();
                return;
            }
            if (!scanFolder(root, stop))
            {
                return;
            }
        }
        pFinder.onLookupFinished();
    }

private:
    // Return false to stop the whole traversal
    bool scanFolder(const std::string& folder, const std::atomic<bool>& stop)
    {
        std::error_code ec;
        std::filesystem::recursive_directory_iterator end;
        std::filesystem::recursive_directory_iterator it(folder, ec);
        if (ec)
        {
            return true;
        }

        for (; it != end; it.increment(ec))
        {
            if (stop.load())
            {
                return false;
            }
            if (ec)
            {
                return false;
            }
            if (!it->is_directory())
            {
                continue;
            }

            const std::string current = it->path().string();
            const StudyVersion versionFound = StudyHeader::tryToFindTheVersion(current);
            if (versionFound == StudyVersion::unknown())
            {
                continue;
            }
            if (versionFound > StudyVersion::latest())
            {
                it.disable_recursion_pending();
                continue;
            }

            // We have found a study !
            pFinder.onStudyFound(current, versionFound);
            it.disable_recursion_pending();
        }
        return !stop.load();
    }

public:
    StudyFinder& pFinder;
};

} // anonymous namespace

StudyFinder::StudyFinder() = default;

StudyFinder::StudyFinder(const StudyFinder&):
    pStopRequested {false}
{
}

StudyFinder::~StudyFinder()
{
    if (pThread.joinable())
    {
        pThread.join();
    }
}

void StudyFinder::stop(unsigned int)
{
    std::lock_guard locker(mutex);
    pStopRequested.store(true);
    if (pThread.joinable())
    {
        pThread.join();
    }
    pStopRequested.store(false);
}

void StudyFinder::wait()
{
    std::lock_guard locker(mutex);
    if (pThread.joinable())
    {
        pThread.join();
    }
}

void StudyFinder::wait(unsigned int)
{
    wait();
}

void StudyFinder::startLookup(const std::vector<std::string>& folders)
{
    std::lock_guard locker(mutex);
    if (pThread.joinable())
    {
        pStopRequested.store(true);
        pThread.join();
    }
    pStopRequested.store(false);

    pThread = std::thread([this, folders]()
                          {
                              Traverser traverser(*this);
                              traverser.run(folders, pStopRequested);
                          });
}

void StudyFinder::lookup(const std::vector<std::string>& folders)
{
    startLookup(folders);
}

void StudyFinder::lookup(const std::list<std::string>& folders)
{
    startLookup(std::vector<std::string>(folders.begin(), folders.end()));
}

void StudyFinder::lookup(const std::string& folder)
{
    startLookup({folder});
}

} // namespace Antares::Data
