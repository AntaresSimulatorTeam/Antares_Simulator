//
// Created by marechaljas on 11/04/23.
//

#include "BindingConstraintTimeSeries.h"

namespace Antares::Data {

    unsigned BindingConstraintTimeSeries::SeriesWidth() const {
        assert(lower_bound_series.width ==
        upper_bound_series.width &&
        upper_bound_series.width ==
        equality_series.width);
        return equality_series.width;
    }

    void BindingConstraintTimeSeries::resize(unsigned int width, unsigned int height) {
        lower_bound_series.resize(width, height);
        upper_bound_series.resize(width, height);
        equality_series.resize(width, height);
    }

    void BindingConstraintTimeSeries::fill(double value) {
        lower_bound_series.fill(value);
        upper_bound_series.fill(value);
        equality_series.fill(value);
    }
} // Data