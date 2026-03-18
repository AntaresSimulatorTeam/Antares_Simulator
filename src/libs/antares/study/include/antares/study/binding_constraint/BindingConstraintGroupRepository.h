// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 28/06/23.
//

#pragma once

#include <functional>
#include <memory>
#include <set>

#include "antares/study/binding_constraint/BindingConstraintGroup.h"
#include "antares/study/fwd.h"

#include "BindingConstraint.h"

namespace Antares::Data
{

class BindingConstraintGroupRepository final
{
public:
    [[nodiscard]] unsigned size() const;

    [[nodiscard]] bool buildFrom(const BindingConstraintsRepository& repository);

    void resizeAllTimeseriesNumbers(unsigned nb_years);

    BindingConstraintGroup* operator[](const std::string& name) const;

    using iterator = std::vector<std::unique_ptr<BindingConstraintGroup>>::iterator;
    using const_iterator = std::vector<std::unique_ptr<BindingConstraintGroup>>::const_iterator;

    [[nodiscard]] iterator begin();
    [[nodiscard]] const_iterator begin() const;

    [[nodiscard]] iterator end();
    [[nodiscard]] const_iterator end() const;

    BindingConstraintGroup* add(const std::string& name);
    void clear();

private:
    [[nodiscard]] bool timeSeriesWidthConsistentInGroups() const;

    std::vector<std::unique_ptr<BindingConstraintGroup>> groups_;
};

} // namespace Antares::Data
