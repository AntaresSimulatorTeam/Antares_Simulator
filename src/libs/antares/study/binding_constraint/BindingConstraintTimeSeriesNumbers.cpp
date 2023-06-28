//
// Created by marechaljas on 27/04/23.
//

#include "BindingConstraintTimeSeriesNumbers.h"

namespace Antares::Data {

    Yuni::uint64 BindingConstraintTimeSeriesNumbers::memoryUsage() const {
        return timeseriesNumbers.memoryUsage();
    }
} // Data