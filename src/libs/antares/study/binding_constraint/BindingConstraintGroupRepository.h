//
// Created by marechaljas on 28/06/23.
//

#pragma once

#include <memory>
#include <set>
#include <functional>
#include "antares/study/binding_constraint/BindingConstraintGroup.h"
#include "antares/study/fwd.h"
#include "BindingConstraint.h"
#include "BindingConstraintSaver.h"

namespace Antares::Data {

class BindingConstraintGroupRepository {
public:
    [[nodiscard]] unsigned size() const;

    [[nodiscard]] bool buildFrom(BindingConstraintsRepository& repository);

    void resizeAllTimeseriesNumbers(unsigned nb_years);

    void fixTSNumbersWhenWidthIsOne();

    BindingConstraintGroup* operator[](std::string name);

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

} // DAta
