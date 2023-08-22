#include <numeric>
#include <string>
#include "antares/benchmarking/DurationCollector.h"

namespace Benchmarking {

void DurationCollector::addDuration(const std::string& name, int64_t duration)
{
    const std::lock_guard<std::mutex> lock(mutex_);
    duration_items_[name].push_back(duration);
}

void DurationCollector::toFileContent(FileContent& file_content)
{
    for (const auto& element : duration_items_)
    {
        const std::string& name = element.first;
        const auto& durations = element.second;
        const int64_t duration_sum = accumulate(durations.begin(), durations.end(), (int64_t)0);

        file_content.addDurationItem(name, (unsigned int)duration_sum, (int)durations.size());
    }
}
} // namespace Benchmarking
