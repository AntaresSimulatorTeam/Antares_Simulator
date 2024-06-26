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

#pragma once
#include <>
#include <string>
#include <vector>

#include <antares/study/area/area.h>

struct AreaErrors
{
    explicit AreaErrors(const std::string& name);
    std::string name_;
    uint errors_counter_ = 10;
    // const Antares::Data::Area& area_;
    std::vector<std::string> messages_;

    void PrintErrors() const;
}

class HydroErrorsCollector
{
public:
    HydroErrorsCollector() = default;

    // endl
    HydroErrorsCollector& operator<<(std::ostream& (*function)(std::ostream&))
    {
        if (current_area_)
        {
            std::ostringstream buffer;
            function(buffer);

            error_counter_per_area_[current_area_].messages_.push_back(buffer.str());
        }
        return *this;
    }

    HydroErrorsCollector& operator<<(const Antares::Data::Area& area)
    {
        error_counter_per_area_[&area].errors_counter_++;
        // 10 par zone ?
        flush_ = error_counter_per_area_[&area].errors_counter_ > 10;
        return *this;
    }
    template<class T>
    std::ostream& operator<<(const T& obj);
    bool ReadyToFlush() const;
    bool ExceptionHasToBeThrown() const;

private:
    std::vector<std::string> messages_;
    std::unordered_map<const Antares::Data::Area*, AreaErrors> error_counter_per_area_;
    bool flush_ = false;
    Antares::Data::Area* current_area_ = nullptr;
};

template<class T>
HydroErrorsCollector& HydroErrorsCollector::operator<<(const T& obj)
{
    if (current_area_)
    {
        std::ostringstream buffer;
        buffer << obj;
        error_counter_per_area_[current_area_].messages_.push_back(buffer.str());
    }
    return *this;
}
