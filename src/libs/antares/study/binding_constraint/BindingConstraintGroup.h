//
// Created by marechaljas on 28/06/23.
//

#pragma once

#include <memory>
#include "BindingConstraint.h"
namespace Antares::Data
{
class BindingConstraintGroup
{
public:
    explicit BindingConstraintGroup(std::string name);

    [[nodiscard]] std::string name()
    {
        return name_;
    }
    void add(const std::shared_ptr<BindingConstraint>& constraint);
    [[nodiscard]] std::set<std::shared_ptr<BindingConstraint>> constraints() const;
    void fixTSNumbersWhenWidthIsOne();
    [[nodiscard]] unsigned numberOfTimeseries() const;

public:
    // Public data members
    Matrix<uint32_t> timeseriesNumbers;

private:
    std::set<std::shared_ptr<BindingConstraint>> constraints_;
    std::string name_;
};

} // namespace Antares::Data
