//
// Created by marechaljas on 28/06/23.
//

#pragma once

#include <memory>
#include <set>
#include "antares/study/binding_constraint/BindingConstraintGroup.h"
#include "antares/study/fwd.h"
namespace Antares::Data {

class BindingConstraintGroupRepository {
public:
    [[nodiscard]] unsigned size() const;

    std::vector<std::shared_ptr<BindingConstraintGroup>> groups_;

    [[nodiscard]] bool buildFrom(BindingConstraintsRepository& repository);

    [[nodiscard]] bool checkTimeSeriesWidthConsistency() const;
};

} // DAta
