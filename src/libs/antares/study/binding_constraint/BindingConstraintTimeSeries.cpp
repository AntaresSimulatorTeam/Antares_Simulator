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

    void BindingConstraintTimeSeries::reset() {
        lesser_than_series.clear();
        greater_than_series.clear();
        equality_series.clear();
        lesser_than_series.markAsModified();
        greater_than_series.markAsModified();
        equality_series.markAsModified();
    }

    bool BindingConstraintTimeSeries::saveToCSVFile(std::string file_name) {
        //TODO
        return false;
    }

    size_t BindingConstraintTimeSeries::memoryUsage() const {
        //TODO
        return 0;
    }

    bool BindingConstraintTimeSeries::forceReload(bool reload) const {
        //TODO
        return false;
    }

    void BindingConstraintTimeSeries::markAsModified() const {
        //TODO
    }

    void BindingConstraintTimeSeries::reset(uint w, uint h, bool fixedSize) {
        //TODO (obnly for type ?)
    }
} // Data