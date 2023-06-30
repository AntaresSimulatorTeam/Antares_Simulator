//
// Created by marechaljas on 28/06/23.
//

#pragma once

#include <memory>
#include "BindingConstraint.h"
namespace Antares::Data {

class BindingConstraintGroup {
public:
    explicit BindingConstraintGroup(std::string name);

    [[nodiscard]] std::string name() { return name_; }
    void add(const std::shared_ptr<BindingConstraint>& constraint);
    [[nodiscard]] std::set<std::shared_ptr<BindingConstraint>> constraints() const;
    [[nodiscard]] BindingConstraintTimeSeriesNumbers& timeSeriesNumbers() { return timeSeriesNumbers_; };
    void fixTSNumbersWhenWidthIsOne();
private:
    std::set<std::shared_ptr<BindingConstraint>> constraints_;
    std::string name_;
    BindingConstraintTimeSeriesNumbers timeSeriesNumbers_;

};

} // Data
