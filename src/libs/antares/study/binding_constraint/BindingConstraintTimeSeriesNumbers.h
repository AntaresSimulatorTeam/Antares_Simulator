//
// Created by marechaljas on 27/04/23.
//

#pragma once

#include "antares/array/matrix.h"
#include "yuni/core/system/stdint.h"

namespace Antares::Data {

class BindingConstraintTimeSeriesNumbers {
public:
    Matrix<Yuni::uint32> timeseriesNumbers;

    Yuni::uint64 memoryUsage() const;
};

    // Antares
} // Data
