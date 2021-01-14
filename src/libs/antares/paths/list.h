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
#ifndef __ANTARES_LIBS_PATHS_LIST_H__
#define __ANTARES_LIBS_PATHS_LIST_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/bind.h>
#include <stdio.h>
#include <map>

enum PathListOption
{
    //! Not found
    pathListOptNotFound = -1,

    /*! No particular option */
    pathListOptNone = 0,
    /*! Flag to indicate that the file must be present */
    pathListOptMustExist = 1,
    /*! Flag to indicate that the file is a folder actually */
    pathListOptFolder = 2,
};

/*!
** \brief Path list structure
*/
class PathList
{
public:
    struct FileInfo
    {
        size_t size;
        enum PathListOption options;
    };
    typedef std::map<Yuni::Clob, FileInfo> ItemList;

    typedef ItemList::iterator iterator;
    typedef ItemList::const_iterator const_iterator;

public:
    PathList()
    {
    }
    ~PathList()
    {
    }

    iterator begin()
    {
        return item.begin();
    }
    const_iterator begin() const
    {
        return item.begin();
    }
    iterator end()
    {
        return item.end();
    }
    const_iterator end() const
    {
        return item.end();
    }

    void clear();

    template<class StringT>
    void add(const StringT& s)
    {
        add(s, pathListOptNone);
    }

    template<class StringT>
    void add(const StringT& s, size_t size)
    {
        add(s, pathListOptNone, size);
    }

    template<class StringT>
    void add(const StringT& s, const PathListOption opt)
    {
        internalPrepare(s);
        if (item.find(pTmp) == item.end())
        {
            FileInfo& info = item[pTmp];
            info.options = opt;
            info.size = 0;
        }
    }

    template<class StringT>
    void add(const StringT& s, const PathListOption opt, size_t size)
    {
        internalPrepare(s);
        if (item.find(pTmp) == item.end())
        {
            FileInfo& info = item[pTmp];
            info.options = opt;
            info.size = size;
        }
    }

    template<class StringT>
    void addFromFolder(const StringT& folder, const PathList& exclude)
    {
        Yuni::Clob s;
        s = folder;
        internalAddFromFolder(s, exclude);
    }

    template<class StringT>
    PathListOption find(const StringT& s) const
    {
        internalPrepare(s);
        ItemList::const_iterator i = item.find(pTmp);
        return ((i != item.end()) ? i->second.options : pathListOptNotFound);
    }

    template<class StringT>
    void remove(const StringT& s)
    {
        if (!item.empty())
        {
            internalPrepare(s);
            ItemList::iterator i = item.find(pTmp);
            if (i != item.end())
                item.erase(i);
        }
    }

    void remove(const PathList& toDelete)
    {
        if (item.empty() || toDelete.empty())
            return;
        const ItemList::const_iterator end = toDelete.item.end();
        for (ItemList::const_iterator i = toDelete.item.begin(); i != end; ++i)
            this->remove(i->first);
    }

    uint size() const
    {
        return (uint)item.size();
    }

    bool empty() const
    {
        return item.empty();
    }

    template<class StringT>
    size_t sizeOnDisk(const StringT& sourceFolder) const
    {
        if (item.empty())
            return 0;
        pTmp = sourceFolder;
        return (!pTmp) ? 0 : internalSizeOnDisk();
    }

    size_t totalSizeInBytes() const;

    template<class StringT>
    uint deleteAllEmptyFolders(const StringT& sourceFolder)
    {
        if (item.empty())
            return 0;
        pTmp = sourceFolder;
        return internalDeleteAllEmptyFolders();
    }

    template<class StringT>
    uint deleteAllFiles(const StringT& sourceFolder)
    {
        if (item.empty())
            return 0;
        pTmp = sourceFolder;
        return internalDeleteAllFiles();
    }

public:
    /*!
    ** \brief Event triggered from time to time
    */
    Yuni::Bind<bool(uint)> onProgress;

private:
    template<class StringT>
    void internalPrepare(const StringT& s) const
    {
        pTmp = s;
        for (uint i = 0; i != pTmp.size(); ++i)
        {
#ifdef YUNI_OS_WINDOWS
            if (pTmp[i] == '/')
                pTmp[i] = '\\';
#else
            if (pTmp[i] == '\\')
                pTmp[i] = '/';
#endif
        }
    }

    size_t internalSizeOnDisk() const;
    uint internalDeleteAllEmptyFolders();
    uint internalDeleteAllFiles();
    void internalAddFromFolder(const Yuni::Clob& folder, const PathList& exclude);

private:
    ItemList item;
    mutable Yuni::Clob pTmp;

}; /* struct PathList */

#endif /* __ANTARES_LIBS_PATHS_LIST_H__ */
