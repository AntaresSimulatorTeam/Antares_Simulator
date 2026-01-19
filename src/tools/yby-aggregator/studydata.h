// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
