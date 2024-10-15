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

#include "antares/paths/list.h"

#include <ctime>

#include <yuni/yuni.h>
#include <yuni/io/directory.h>
#include <yuni/io/directory/iterator.h>
#ifdef YUNI_OS_WINDOWS
#include <direct.h>
#endif
#include <antares/logs/logs.h>

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

void PathList::clear()
{
    item.clear();
}

size_t PathList::totalSizeInBytes() const
{
    size_t size = 0;

    const ItemList::const_iterator end = item.end();
    for (ItemList::const_iterator i = item.begin(); i != end; ++i)
    {
        size += i->second.size;
    }
    return size;
}

// TODO VP: remove with tools
uint PathList::internalDeleteAllEmptyFolders()
{
    if (!pTmp || item.empty())
    {
        return 0;
    }

    Clob buffer;
    uint count = 0;

    const ItemList::const_reverse_iterator end = item.rend();
    for (ItemList::const_reverse_iterator i = item.rbegin(); i != end; ++i)
    {
        // The item must be flagged as a folder
        if (i->second.options & pathListOptFolder)
        {
            buffer.clear() << pTmp << SEP << i->first;

            if (IO::Directory::Remove(buffer) || !IO::Directory::Exists(buffer))
            {
                ++count;
            }
            else
            {
                logs.warning() << "I/O error: could not remove " << buffer;
            }
        }
    }
    return count;
}

uint PathList::internalDeleteAllFiles()
{
    if (!pTmp || item.empty())
    {
        return 0;
    }

    Clob buffer;
    uint count = 0;

    const ItemList::const_iterator end = item.end();
    for (ItemList::const_iterator i = item.begin(); i != end; ++i)
    {
        // The item must _not_ be flagged as a folder
        if (0 == (i->second.options & pathListOptFolder))
        {
            buffer.clear() << pTmp << SEP << i->first;
            if (IO::File::Delete(buffer) || !IO::File::Exists(buffer))
            {
                ++count;
            }
            else
            {
                logs.warning() << "I/O error: could not delete " << buffer;
            }
        }
    }
    return count;
}

class PathListIterator: public IO::Directory::IIterator<true>
{
public:
    using IteratorType = IO::Directory::IIterator<true>;
    using Flow = IO::Flow;

public:
    PathListIterator(PathList& l, const PathList& e, std::function<bool(uint)>& progress):
        list(l),
        exclude(e),
        onProgress(progress)
    {
    }

    virtual ~PathListIterator()
    {
        // For code robustness and to avoid corrupt vtable
        stop();
    }

protected:
    virtual bool onStart(const String& rootFolder)
    {
        offset = rootFolder.size() + 1;
        return true;
    }

    virtual Flow onBeginFolder(const String& filename, const String&, const String&)
    {
        if (pathListOptNotFound == exclude.find(filename.c_str() + offset))
        {
            list.add(filename.c_str() + offset, pathListOptFolder);
            return onProgress(list.size()) ? IO::flowContinue : IO::flowAbort;
        }
        return IO::flowSkip;
    }

    virtual Flow onFile(const String& filename, const String&, const String&, uint64_t size)
    {
        if (pathListOptNotFound == exclude.find(filename.c_str() + offset))
        {
            list.add(filename.c_str() + offset, (size_t)size);
        }
        return IO::flowContinue;
    }

private:
    PathList& list;
    const PathList& exclude;
    uint offset;
    std::function<bool(uint)>& onProgress;
};

void PathList::internalAddFromFolder(const Clob& folder, const PathList& exclude)
{
    PathListIterator iterator(*this, exclude, onProgress);
    iterator.add(folder.c_str());
    iterator.start();
    iterator.wait();
}
