// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_FS_WALKER_REGISTRY_H__
#define __ANTARES_FS_WALKER_REGISTRY_H__

#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

namespace FSWalker
{
class EventsRegistry: private Yuni::NonCopyable<EventsRegistry>
{
public:
    using OnDirectoryEventList = std::vector<OnDirectoryEvent>;
    using OnFileEventList = std::vector<OnFileEvent>;
    using IndexList = std::vector<uint>;
    using UserDataList = std::vector<void*>;

public:
    EventsRegistry()
    {
    }

    ~EventsRegistry();
    void initialize(const IExtension::Vector& exts, DispatchJobEvent& queue);
    void finalize();

    struct
    {
        OnDirectoryEventList enter;
        IndexList indexes;
        UserDataList userdata;
    } directory;

    struct
    {
        OnFileEventList access;
        IndexList indexes;
        UserDataList userdata;
    } file;

    //! Extensions
    IExtension::Vector extensions;
    //! Unique user data per extension
    UserDataList uniqueUserdata;

}; // class EventsRegistry

inline EventsRegistry::~EventsRegistry()
{
    finalize();
}

void EventsRegistry::finalize()
{
    if (extensions.empty())
    {
        return;
    }

    for (uint i = 0; i != extensions.size(); ++i)
    {
        void* userdata = uniqueUserdata[i];
        if (!userdata)
        {
            continue;
        }
        auto& extension = *(extensions[i]);

        // release ressources
        extension.userdataDestroy(userdata);
        uniqueUserdata[i] = nullptr;
    }

    file.indexes.clear();
    file.access.clear();
    file.userdata.clear();

    directory.indexes.clear();
    directory.enter.clear();
    directory.userdata.clear();

    extensions.clear();
}

void EventsRegistry::initialize(const IExtension::Vector& exts, DispatchJobEvent& queue)
{
    // release all previously acquired ressources
    finalize();
    // Keeping a reference on each extension
    extensions = exts;
    uniqueUserdata.resize(extensions.size());

    for (uint i = 0; i != extensions.size(); ++i)
    {
        auto& extension = *(extensions[i]);

        // File access
        auto access = extension.fileEvent();
        // Directory access
        auto directoryEnter = extension.directoryEvent();

        if (!access && !directoryEnter)
        {
            uniqueUserdata[i] = nullptr;
            continue;
        }

        void* userdata = extension.userdataCreate(queue);
        uniqueUserdata[i] = userdata;

        if (access)
        {
            file.indexes.push_back(i);
            file.access.push_back(access);
            file.userdata.push_back(userdata);
        }

        if (directoryEnter)
        {
            directory.indexes.push_back(i);
            directory.enter.push_back(directoryEnter);
            directory.userdata.push_back(userdata);
        }

    } // each extension
}

} // namespace FSWalker

#endif // __ANTARES_FS_WALKER_REGISTRY_H__
