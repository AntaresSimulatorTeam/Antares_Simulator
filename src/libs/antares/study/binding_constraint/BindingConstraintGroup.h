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

    [[nodiscard]] std::string name();

    void add(const std::shared_ptr<BindingConstraint>& constraint);
    [[nodiscard]] const BindingConstraint::Set& constraints() const;
    [[nodiscard]] unsigned numberOfTimeseries() const;

public:
    // Public data members
    Matrix<uint32_t> timeseriesNumbers;

private:
    BindingConstraint::Set constraints_;
    std::string name_;
};

} // namespace Antares::Data
