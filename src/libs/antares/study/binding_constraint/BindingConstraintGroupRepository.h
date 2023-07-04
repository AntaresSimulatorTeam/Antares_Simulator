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

    [[nodiscard]] bool timeSeriesWidthConsistentInGroups() const;
    void resizeAllTimeseriesNumbers(unsigned nb_years);

    void fixTSNumbersWhenWidthIsOne();

    std::shared_ptr<BindingConstraintGroup> operator[](std::string name);

    [[nodiscard]] std::vector<std::shared_ptr<BindingConstraintGroup>>::iterator begin();
    [[nodiscard]] std::vector<std::shared_ptr<BindingConstraintGroup>>::const_iterator begin() const;

    [[nodiscard]] std::vector<std::shared_ptr<BindingConstraintGroup>>::iterator end();
    [[nodiscard]] std::vector<std::shared_ptr<BindingConstraintGroup>>::const_iterator end() const;

    std::shared_ptr<BindingConstraintGroup> add(const std::string& name);
    void clear();

private:
    std::vector<std::shared_ptr<BindingConstraintGroup>> groups_;
};

} // DAta
