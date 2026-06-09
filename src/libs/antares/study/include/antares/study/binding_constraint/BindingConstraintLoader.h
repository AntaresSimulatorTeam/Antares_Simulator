// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

class BindingConstraintLoader final
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

    void populateConstraint(const EnvForLoading& env, std::shared_ptr<BindingConstraint>& bc);

    void parseWeightAndOffset(const EnvForLoading& env,
                              const IniFile::Property* p,
                              std::shared_ptr<BindingConstraint>& bc);

    bool validate(const EnvForLoading& env, const std::shared_ptr<BindingConstraint>& bc);

    std::vector<std::shared_ptr<BindingConstraint>> loadByOperator(
      EnvForLoading& env,
      std::shared_ptr<BindingConstraint>& bc);
};

} // namespace Antares::Data
