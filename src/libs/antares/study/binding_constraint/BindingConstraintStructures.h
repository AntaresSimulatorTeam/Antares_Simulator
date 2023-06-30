//
// Created by marechaljas on 27/06/23.
//

#pragma once

#include <vector>

namespace Antares {
    namespace Data {

        struct BindingConstraintStructures {
            mutable std::vector<long>   linkIndex;
            mutable std::vector<double> linkWeight;
            mutable std::vector<double> clusterWeight;
            mutable std::vector<int>    linkOffset;
            mutable std::vector<int>    clusterOffset;
            mutable std::vector<long>   clusterIndex;
            mutable std::vector<long>   clustersAreaIndex;
        };

    } // Antares
} // Data
