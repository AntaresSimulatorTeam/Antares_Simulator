/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <vector>
#include <string>
#include <utility> // for std::move

enum class Location {
    MASTER,
    MASTER_AND_SUBPROBLEMS,
    SUBPROBLEMS
};

class Variable {
public:
    std::string id;
    Location location;
};

class Objective {
public:
    std::string id;
    Location location;
};

class ModelDecomposition {
public:
    std::vector<Variable> variables;
    std::vector<Objective> objectives;
};

class OptimConfig {
public:
    std::string id;
    ModelDecomposition modelDecomposition;

    // Friend declaration
    friend class OptimConfigBuilder;
};

class OptimConfigBuilder
{
private:
    OptimConfig config_;

public:
    OptimConfigBuilder& withId(const std::string& id) {
        config_.id = id;
        return *this;
    }

    OptimConfigBuilder& withVariables(std::vector<Variable>&& variables) {
        config_.modelDecomposition.variables = std::move(variables);
        return *this;
    }

    OptimConfigBuilder& withObjectives(std::vector<Objective>&& objectives) {
        config_.modelDecomposition.objectives = std::move(objectives);
        return *this;
    }

    OptimConfig build() {
        return config_;
    }
};

