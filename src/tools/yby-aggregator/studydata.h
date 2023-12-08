/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __STUDY_STUDY_DATA_AGGREGATOR_STUDY_DATA_H__
#define __STUDY_STUDY_DATA_AGGREGATOR_STUDY_DATA_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <memory>
#include <set>

class StudyData final
{
public:
    //! The most suitable smart pointer
    using Ptr = std::shared_ptr<StudyData>;
    //! Vector
    using Vector = std::vector<Ptr>;
    //! Short string
    using ShortString = Yuni::CString<10, false>;
    //! Short string
    using ShortString512 = Yuni::CString<512, false>;

public:
    template<class StringT>
    StudyData(const StringT& n, uint i) : index(i), name(n)
    {
    }

    template<class StringT>
    void append(StringT& out) const
    {
        out << name;
    }

public:
    //! Local index
    uint index;
    //! type + Name of the area or the link
    const ShortString512 name;

}; // class StudyData

#endif // __STUDY_STUDY_DATA_AGGREGATOR_STUDY_DATA_H__
