#include "antares/solver/hydro/management/HydroErrorsCollector.h"

bool HydroErrorsCollector::ReadyToFlush() const
{
    return flush_;
}

bool HydroErrorsCollector::ExceptionHasToBeThrown() const
{
    return !error_counter_per_area_.empty();
}
