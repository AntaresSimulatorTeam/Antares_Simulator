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
#ifndef __ANTARES_FS_WALKER_REGISTRY_H__
#define __ANTARES_FS_WALKER_REGISTRY_H__

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include <vector>

namespace FSWalker
{
class EventsRegistry : private Yuni::NonCopyable<EventsRegistry>
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
        return;

    for (uint i = 0; i != extensions.size(); ++i)
    {
        void* userdata = uniqueUserdata[i];
        if (!userdata)
            continue;
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
