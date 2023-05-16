//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <memory>
#include "antares/study/area/area.h"

namespace Antares::Data {

class BindingConstraint;
class BindingConstraintLoader {
public:
    std::vector<std::shared_ptr<BindingConstraint>> load(EnvForLoading env);

private:
    static bool SeparateValue(const EnvForLoading &env, const IniFile::Property *p, double &w, int &o);

    bool loadTimeSeries(EnvForLoading &env, BindingConstraint *bindingConstraint);
    bool loadTimeSeriesBefore860(EnvForLoading &env, BindingConstraint *bindingConstraint);
    bool loadBoundedTimeSeries(EnvForLoading &env, BindingConstraint::Operator op, BindingConstraint *bindingConstraint);
};

} // Data
