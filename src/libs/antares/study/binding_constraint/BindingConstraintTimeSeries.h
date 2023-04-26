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
    BindingConstraintTimeSeries(const BindingConstraintTimeSeries&) = default;
    //Width = number of chronicles
    //Height = number of time step
    Matrix<double> lesser_than_series;
    Matrix<double> greater_than_series;
    Matrix<double> equality_series;
    Matrix<Yuni::uint32> timeseriesNumbers;
    unsigned SeriesWidth() const;
    void resize(unsigned width, unsigned height);
    void fill(double value);
};

} // Data
