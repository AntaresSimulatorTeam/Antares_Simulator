#include "antares/solver/hydro/management/HydroErrorsCollector.h"

#include <yuni/core/logs.h>

void HydroErrorsCollector::IncreaseCounterForArea(const Antares::Data::Area* area)
{
    if (errors_limit_reached_ || !area)
    {
        return;
    }

    area_errors_counter_[area]++;
    errors_limit_reached_ = area_errors_counter_[area] > 10 || errors_limit_reached_;
    stop_ = true;
}

bool HydroErrorsCollector::ErrorsLimitReached() const
{
    return errors_limit_reached_;
}

bool HydroErrorsCollector::StopExecution() const
{
    return stop_ || errors_limit_reached_;
}

void HydroErrorsCollector::FatalErrorHit()
{
    stop_ = true;
}

// void HydroErrorsCollector::RecordFatalErrors(const std::string& msg, uint year)
// {
//     fatal_errors_.push_back("In year " + std::to_string(year) + " " + msg);
// }

// void HydroErrorsCollector::RecordFatalErrors(const std::string& msg,
//                                              uint year,
//                                              const Antares::Data::Area* area)
// {
//     fatal_errors_.push_back("In area " + area.name + " " + std::to_string(year) + " " + msg);
// }

// void HydroErrorsCollector::PrintFatalsErrors() const
// {
//     for (const auto& msg: fatal_errors)
//     {
//         logs.error() << msg;
//     }
// }
