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

    [[nodiscard]] bool buildFrom(const BindingConstraintsRepository& repository);

    void resizeAllTimeseriesNumbers(unsigned nb_years);

    BindingConstraintGroup* operator[](const std::string& name) const;

    using iterator = std::vector<BindingConstraintGroup*>::iterator;
    using const_iterator = std::vector<BindingConstraintGroup*>::const_iterator;

    [[nodiscard]] iterator begin();
    [[nodiscard]] const_iterator begin() const;

    [[nodiscard]] iterator end();
    [[nodiscard]] const_iterator end() const;

    BindingConstraintGroup* add(const std::string& name);
    void clear();

private:
    [[nodiscard]] bool timeSeriesWidthConsistentInGroups() const;

    /**
     * Owning vector of groups
     */
    std::vector<std::unique_ptr<BindingConstraintGroup>> owning_groups_;
    /**
     * Non-owning vector of groups. Used for accessing data
     */
    std::vector<BindingConstraintGroup*> groups_;
    /**
     * Used to speed up the search for a group by name
     * Debatable: we could use a map and discard "groups_" vector
     * Would require performance analysis
     */
    mutable std::unordered_map<std::string, BindingConstraintGroup*> groupsByName_;
};

} // Data
