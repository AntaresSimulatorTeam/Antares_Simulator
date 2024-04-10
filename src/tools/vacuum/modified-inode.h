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
#ifndef __MODIFIED_INODE_H__
#define __MODIFIED_INODE_H__

#include <fswalker/fswalker.h>
#include <mutex>

class ModifiedINode: public FSWalker::IExtension
{
public:
    ModifiedINode(int64_t dateLimit);

    virtual ~ModifiedINode()
    {
    }

    virtual const char* caption() const;

    virtual FSWalker::OnDirectoryEvent directoryEvent();
    virtual FSWalker::OnFileEvent fileEvent();

    virtual void* userdataCreate(FSWalker::DispatchJobEvent& queue);
    virtual void userdataDestroy(void* userdata);

public:
    uint64_t bytesDeleted;
    uint64_t filesDeleted;
    uint64_t foldersDeleted;

private:
    std::mutex pMutex;
    int64_t pDateLimit;
    FSWalker::DispatchJobEvent pQueue;

}; // class ModifiedINode

#endif // __MODIFIED_INODE_H__
