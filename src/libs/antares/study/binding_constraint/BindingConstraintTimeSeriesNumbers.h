//
// Created by marechaljas on 27/04/23.
//

#pragma once

#include "antares/array/matrix.h"

namespace Antares::Data {

        class BindingConstraintTimeSeriesNumbers {
        public:
            Matrix<Yuni::uint32> timeseriesNumbers;
            unsigned SeriesWidth() const;
        };

    // Antares
} // Data
