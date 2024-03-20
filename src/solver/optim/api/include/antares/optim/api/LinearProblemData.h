/*
** Copyright 2007-2024 RTE
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
#pragma once

#include <utility>
#include <vector>
#include <functional>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::optim::api
{
class LinearProblemData final
{
public:
    template<class V>
    class GroupedData
    {
    public:
        GroupedData() = default;
        template<class T>
        GroupedData(const T& v) : values({v})
        {
        }
        template<class T>
        GroupedData(T&& v) : values({v})
        {
        }
        template<class T>
        GroupedData(std::initializer_list<T> v) : values({v})
        {
        }

        template<class T>
        GroupedData(std::string group, std::initializer_list<T> values) :
         group(group), values(values)
        {
        }

        inline V& operator[](std::size_t idx)
        {
            return values[idx];
        }

        inline const V& operator[](std::size_t idx) const
        {
            return values[idx];
        }

        std::string group;
        std::vector<V> values; // index = tsIndex
    };

    using ScalarData = GroupedData<double>;
    using ScalarDataDict = std::map<std::string, ScalarData>;

    using TimedData = GroupedData<std::vector<double>>;
    using TimedDataDict = std::map<std::string, TimedData>;

    using GroupYearToIndex
      = std::map<std::string /*group*/, std::map<unsigned int /*year*/, unsigned int /*tsIndex*/>>;

    // TODO : timestamps or timesteps?
    std::vector<int> timeStamps_;
    int timeResolutionInMinutes_;

    ScalarDataDict scalarData_;
    TimedDataDict timedData_;
    // TODO : handle scenarios, and data vectorized on scenarios, on time, or on both
    GroupYearToIndex groupYearToIndex_;

public:
    explicit LinearProblemData(const std::vector<int>& timeStamps,
                               int timeResolutionInMinutes,
                               const ScalarDataDict& scalarData,
                               const TimedDataDict& timedData,
                               const GroupYearToIndex& groupToYear = {{"", {{0, 0}}}}) :
     timeStamps_(timeStamps),
     timeResolutionInMinutes_(timeResolutionInMinutes),
     scalarData_(scalarData),
     timedData_(timedData),
     groupYearToIndex_(groupToYear){
       // TODO: some coherence check on data
       // for example, check that timed data are all of same size = size of timeStamps_
     };
    [[nodiscard]] std::vector<int> getTimeStamps() const
    {
        return timeStamps_;
    }
    [[nodiscard]] int getTimeResolutionInMinutes() const
    {
        return timeResolutionInMinutes_;
    }

    // TODO use cache ?
    inline std::set<std::string> groups() const
    {
        std::set<std::string> groups;
        for (const auto& [_, scalar] : scalarData_)
            groups.insert(scalar.group);

        for (const auto& [_, timed] : timedData_)
            groups.insert(timed.group);

        return groups;
    }

    struct Legacy
    {
        const std::vector<CORRESPONDANCES_DES_CONTRAINTES>* constraintMapping;
        const std::vector<const char*>* areaNames;
    };

    class YearView
    {
    public:
        YearView(const LinearProblemData& data, unsigned year) :
         legacy(data.legacy),
         timeStamps_(data.timeStamps_),
         timeResolutionInMinutes_(data.timeResolutionInMinutes_)
        {
            for (const auto& group : data.groups())
            {
                unsigned tsIndex = data.groupYearToIndex_.at(group).at(year);
                for (auto& [key, scalarData] : data.scalarData_)
                {
                    if (scalarData.group == group)
                    {
                        scalarData_.insert({key, scalarData[tsIndex]});
                    }
                }

                for (auto& [key, timedData] : data.timedData_)
                {
                    if (timedData.group == group)
                    {
                        timedData_.insert({key, std::cref(timedData[tsIndex])});
                    }
                }
            }
        }
        [[nodiscard]] bool hasScalarData(const std::string& key) const
        {
            return scalarData_.contains(key);
        }
        [[nodiscard]] double getScalarData(const std::string& key) const
        {
            return scalarData_.at(key);
        }
        [[nodiscard]] bool hasTimedData(const std::string& key) const
        {
            return timedData_.contains(key);
        }
        [[nodiscard]] const std::vector<double>& getTimedData(const std::string& key) const
        {
            return timedData_.at(key);
        }

        [[nodiscard]] const std::vector<int>& getTimeStamps() const
        {
            return timeStamps_;
        }

        [[nodiscard]] int getTimeResolutionInMinutes() const
        {
            return timeResolutionInMinutes_;
        }

    public:
        const Legacy& legacy;
        const std::vector<int>& timeStamps_;
        const double timeResolutionInMinutes_;

    private:
        std::map<std::string, double> scalarData_;
        std::map<std::string, std::reference_wrapper<const std::vector<double>>> timedData_;
    };

    YearView operator[](std::size_t year) const
    {
        return YearView(*this, year);
    }

    // TODO: remove this when legacy support is dropped
    // TODO: meanwhile, instead of having a nested struct, create a daughter class?
    Legacy legacy;
    // TODO[FOM] Move as argument ?
    // TODO[FOM] No default value ?
    unsigned int year_ = 0;
};
} // namespace Antares::optim::api
