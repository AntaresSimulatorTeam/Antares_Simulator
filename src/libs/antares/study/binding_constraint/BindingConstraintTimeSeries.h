//
// Created by marechaljas on 11/04/23.
//

#pragma once

#include "antares/array/matrix.h"

namespace Antares::Data {

class BindingConstraintTimeSeries {
public:
    BindingConstraintTimeSeries() = default;
    ~BindingConstraintTimeSeries() = default;
    BindingConstraintTimeSeries(BindingConstraintTimeSeries&) = default;
    Matrix<double> lower_bound_series;
    Matrix<double> upper_bound_series;
    Matrix<double> equality_series;
    Matrix<Yuni::uint32> timeseriesNumbers;
    unsigned SeriesWidth() const;
    void resize(unsigned width, unsigned height);
};

} // Data
