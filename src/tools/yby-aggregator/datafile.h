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
