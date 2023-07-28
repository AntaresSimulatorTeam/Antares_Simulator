//
// Created by marechaljas on 28/06/23.
//

#include "BindingConstraintGroup.h"

#include <algorithm>
#include <utility>

namespace Antares::Data {

    void BindingConstraintGroup::add(const std::shared_ptr<BindingConstraint> &constraint) {
        constraints_.insert(constraint);
    }

    BindingConstraintGroup::BindingConstraintGroup(std::string name) :
            name_(std::move(name)) {

    }

    std::set<std::shared_ptr<BindingConstraint>> BindingConstraintGroup::constraints() const {
        return constraints_;
    }

    void BindingConstraintGroup::fixTSNumbersWhenWidthIsOne() {
        if (std::all_of(constraints_.begin(), constraints_.end(), [](auto constraint){
            return constraint->RHSTimeSeries().width == 1;
        })) {
            timeSeriesNumbers_.fillColumn(0, 0);
        }
    }

    unsigned BindingConstraintGroup::numberOfTimeseries() const {
        //Assume all BC in a group have the same width
        if (constraints_.empty()) return 0;
        return (*constraints_.begin())->RHSTimeSeries().width;
    }

} // Data