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
#ifndef __STUDY_STUDY_DATA_AGGREGATOR_STUDY_DATA_H__
#define __STUDY_STUDY_DATA_AGGREGATOR_STUDY_DATA_H__

#include <memory>
#include <set>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

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
    StudyData(const StringT& n, uint i):
        index(i),
        name(n)
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
