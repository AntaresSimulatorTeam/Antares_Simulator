/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <memory>

#include "antares/study/area/area.h"

#include "BindingConstraint.h"
#include "EnvForLoading.h"

namespace Antares::Data
{

class BindingConstraint;

class BindingConstraintLoader
{
public:
    std::vector<std::shared_ptr<BindingConstraint>> load(EnvForLoading env);

private:
    static bool SeparateValue(const EnvForLoading& env,
                              const IniFile::Property* p,
                              double& w,
                              int& o);

    bool loadTimeSeries(EnvForLoading& env, BindingConstraint* bindingConstraint);
    bool loadTimeSeriesLegacyStudies(EnvForLoading& env,
                                     BindingConstraint* bindingConstraint) const;
    bool loadTimeSeries(EnvForLoading& env,
                        BindingConstraint::Operator operatorType,
                        BindingConstraint* bindingConstraint) const;
};

} // namespace Antares::Data
