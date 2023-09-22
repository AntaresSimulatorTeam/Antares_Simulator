//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <memory>
#include "antares/study/area/area.h"
#include "EnvForLoading.h"
#include "BindingConstraint.h"

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
