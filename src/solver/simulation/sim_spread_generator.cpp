#include "sim_spread_generator.h"

namespace SIM
{
SpreadGenerator::SpreadGenerator(double range) : range_(range)
{
}

void SpreadGenerator::reset(unsigned int seed)
{
    mt_.reset(seed);
}

double SpreadGenerator::generate()
{
    return mt_.next() * range_;
}
} // namespace SIM
