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
#ifndef __STUDY_DATA_FILE_AGGREGATOR_DATA_FILE_H__
#define __STUDY_DATA_FILE_AGGREGATOR_DATA_FILE_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <set>

class DataFile final
{
public:
    //! The most suitable smart pointer
    typedef Yuni::SmartPtr<DataFile> Ptr;
    //! Vector
    typedef std::vector<Ptr> Vector;
    //! Short string
    typedef Yuni::CString<10, false> ShortString;

public:
    template<class StringT, class StringT2>
    DataFile(const StringT& data, const StringT2& time, uint i) :
     index(i), dataLevel(data), timeLevel(time)
    {
    }

    template<class StringT>
    void append(StringT& out) const
    {
        out << dataLevel << '-' << timeLevel << ".txt";
    }

public:
    //! Data target index
    const uint index;
    //! Data level (values, details, id...)
    const ShortString dataLevel;
    //! File level (hourly, daily...)
    const ShortString timeLevel;

}; // class DataFile

#endif // __STUDY_DATA_FILE_AGGREGATOR_DATA_FILE_H__
