// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __STUDY_DATA_FILE_AGGREGATOR_DATA_FILE_H__
#define __STUDY_DATA_FILE_AGGREGATOR_DATA_FILE_H__

#include <memory>
#include <set>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

class DataFile final
{
public:
    //! The most suitable smart pointer
    using Ptr = std::shared_ptr<DataFile>;
    //! Vector
    using Vector = std::vector<Ptr>;
    //! Short string
    using ShortString = Yuni::CString<10, false>;

public:
    template<class StringT, class StringT2>
    DataFile(const StringT& data, const StringT2& time, uint i):
        index(i),
        dataLevel(data),
        timeLevel(time)
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
