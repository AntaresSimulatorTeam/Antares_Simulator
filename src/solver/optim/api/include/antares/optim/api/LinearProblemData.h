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

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::optim::api
{
    class LinearProblemData final
    {
    public:
        using ScalarDataDict = std::map<std::string, double>;

        using TimedData = std::vector<double>;
        using TimedDataDict = std::map<std::string, TimedData>;

    private:
        // TODO : timestamps or timesteps?
        std::vector<int> timeStamps_;
        int timeResolutionInMinutes_;

        ScalarDataDict scalarData_;
        TimedDataDict timedData_;
        // TODO : handle scenarios, and data vectorized on scenarios, on time, or on both
    public:
        explicit LinearProblemData(const std::vector<int> &timeStamps, int timeResolutionInMinutes,
                                   const ScalarDataDict& scalarData,
                                   const TimedDataDict& timedData) :
                timeStamps_(timeStamps), timeResolutionInMinutes_(timeResolutionInMinutes),
                scalarData_(scalarData), timedData_(timedData)
        {
            // TODO: some coherence check on data
            // for example, check that timed data are all of same size = size of timeStamps_
        };
        [[nodiscard]] std::vector<int> getTimeStamps() const { return timeStamps_; }
        [[nodiscard]] int getTimeResolutionInMinutes() const { return timeResolutionInMinutes_; }
        [[nodiscard]] bool hasScalarData(const std::string& key) const { return scalarData_.contains(key); }
        [[nodiscard]] double getScalarData(const std::string& key) const { return scalarData_.at(key); }
        [[nodiscard]] bool hasTimedData(const std::string& key) const { return timedData_.contains(key); }
        [[nodiscard]] const TimedData& getTimedData(const std::string& key) const { return timedData_.at(key); }

        // TODO: remove this when legacy support is dropped
        // TODO: meanwhile, instead of having a nested struct, create a daughter class?
        struct Legacy {
            const std::vector<CORRESPONDANCES_DES_CONTRAINTES>* constraintMapping;
            const std::vector<const char*>* areaNames;
        };
        Legacy legacy;
    };

}
