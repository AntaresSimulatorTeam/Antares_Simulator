// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __STUDY_OUTPUT_AGGREGATOR_OUTPUT_H__
#define __STUDY_OUTPUT_AGGREGATOR_OUTPUT_H__

#include <atomic>
#include <memory>
#include <set>

#include <yuni/core/string.h>

#include "result.h"

class Output final
{
public:
    //! The most suitable smart pointer
    using Ptr = std::shared_ptr<Output>;
    //! A folder name (short length)
    using FolderName = Yuni::CString<10, false>;
    //! Column name
    using ColumnName = Yuni::CString<128, false>;
    //! Vector
    using Vector = std::vector<Ptr>;

public:
    Output(const YString& target, const YString::Vector& cols):
        path(target),
        columns(cols)
    {
    }

    bool canContinue() const
    {
        return (errors < 100);
    }

    void incrementError();

public:
    //! The minimum value for the years
    uint minYear;
    //! The maximum value for the years
    uint maxYear;
    //! The total number of years
    uint nbYears;
    //! The study output directory
    const Yuni::String path;
    //! All columns to extract
    const Yuni::String::Vector columns;
    //! The number of errors
    std::atomic<int> errors;

    //! Results
    ResultsForAllStudyItems results;

}; // class Output

#endif // __STUDY_OUTPUT_AGGREGATOR_OUTPUT_H__
