// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
