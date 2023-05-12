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
    std::vector<std::shared_ptr<BindingConstraint>> load(EnvForLoading env, unsigned int years) const;
};

} // Data
