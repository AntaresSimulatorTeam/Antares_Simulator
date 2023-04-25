#pragma once

namespace Antares::Data::ShortTermStorage
{

    class Bounds
    {
    public:
        Bounds(double l, double u) : lower(l), upper(u) {}
        double lower;
        double upper;
    };
} // End Antares::Data::ShortTermStorage
