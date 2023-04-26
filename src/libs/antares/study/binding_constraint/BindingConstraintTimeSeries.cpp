//
// Created by marechaljas on 11/04/23.
//

#include "BindingConstraintTimeSeries.h"

namespace Antares::Data {

    unsigned BindingConstraintTimeSeries::SeriesWidth() const {
        assert(lesser_than_series.width ==
               greater_than_series.width &&
               greater_than_series.width ==
               equality_series.width);
        return equality_series.width;
    }

    void BindingConstraintTimeSeries::resize(unsigned int width, unsigned int height) {
        lesser_than_series.resize(width, height);
        greater_than_series.resize(width, height);
        equality_series.resize(width, height);
    }

    void BindingConstraintTimeSeries::fill(double value) {
        lesser_than_series.fill(value);
        greater_than_series.fill(value);
        equality_series.fill(value);
    }
} // Data