// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/utils/utils.h"

#include <sstream>

#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares
{

template<>
void TransformNameIntoID(const AnyString& name, std::string& out)
{
    Yuni::String yuniOut;
    TransformNameIntoID(name, yuniOut);
    out = yuniOut;
}

std::string transformNameIntoID(const std::string& name)
{
    std::string out;
    TransformNameIntoID(name, out);
    return out;
}

std::tm getCurrentTime()
{
    using namespace std::chrono;
    auto now = system_clock::to_time_t(system_clock::now());
    std::tm local_time;
#ifdef _WIN32
    localtime_s(&local_time, &now); // Windows
#else
    localtime_r(&now, &local_time); // POSIX
#endif
    return local_time;
}

std::string formatTime(const std::tm& localTime, const std::string& format)
{
    char timeBuffer[256];
    std::strftime(timeBuffer, sizeof(timeBuffer), format.c_str(), &localTime);

    return std::string(timeBuffer);
}

std::vector<std::pair<std::string, std::string>> splitStringIntoPairs(const std::string& s,
                                                                      char delimiter1,
                                                                      char delimiter2)
{
    std::vector<std::pair<std::string, std::string>> pairs;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter1))
    {
        size_t pos = token.find(delimiter2);
        if (pos != std::string::npos)
        {
            std::string begin = token.substr(0, pos);
            std::string end = token.substr(pos + 1);
            pairs.emplace_back(begin, end);
        }
        else
        {
            logs.warning() << "Error while parsing: " << token;
            logs.warning() << "Correct format is: \"object1" << delimiter2 << "object2"
                           << delimiter1 << "object3" << delimiter2 << "object4\"";
        }
    }

    return pairs;
}

namespace Utils
{

bool isZero(double d)
{
    constexpr double threshold = 1.e-6;
    return std::abs(d) < threshold;
}

double round(double d, unsigned precision)
{
    auto factor = std::pow(10, precision);
    return std::round(d * factor) / factor;
}

static constexpr double largeValue = 1000000;

double ceil(double d)
{
    return std::ceil(std::round(d * largeValue) / largeValue);
}

double floor(double d)
{
    return std::floor(std::round(d * largeValue) / largeValue);
}

bool isPathValid([[maybe_unused]] const std::string& path)
{
#if defined(_WIN32)
    return std::ranges::all_of(path, [](unsigned c) { return c <= 127; });
#else
    return true;
#endif
}

std::map<std::string, unsigned> giveNumbersToStrings(const std::vector<std::string>& strs)
{
    unsigned strNb = 0;
    std::map<std::string, unsigned> strToNumber;
    for (const auto& s: strs)
    {
        strToNumber[s] = strNb++;
    }
    return strToNumber;
}

std::map<std::string, unsigned> giveNumbersToStrings(const std::set<std::string>& strs)
{
    unsigned strNb = 0;
    std::map<std::string, unsigned> strToNumber;
    for (const auto& s: strs)
    {
        strToNumber[s] = strNb++;
    }
    return strToNumber;
}

bool checkAllElementsIdenticalOrOne(std::vector<unsigned> w)
{
    auto first_one = std::remove(w.begin(), w.end(), 1); // Reject all 1 to the end
    return std::adjacent_find(w.begin(), first_one, std::not_equal_to<unsigned>()) == first_one;
}

bool checkAllElementsIdenticalOrOne(std::vector<std::pair<unsigned, std::string>>& p)
{
    // Erase 1 from the vector
    std::erase_if(p, [](const auto& pair) { return pair.first == 1; });
    if (p.empty())
    {
        return true;
    }

    auto width = p.begin()->first;
    for (const auto& [w, msg]: p)
    {
        if (w != width)
        {
            logs.error() << "Inconsitent time series width, found: " << w << " Previous was "
                         << width << " for " << msg;

            return false;
        }
    }
    return true;
}

TimeMeasurement::TimeMeasurement()
{
    reset();
}

void TimeMeasurement::tick()
{
    end_ = clock::now();
}

long TimeMeasurement::duration_ms() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
}

std::string TimeMeasurement::toString() const
{
    return std::to_string(duration_ms()) + " ms";
}

std::string TimeMeasurement::toStringInSeconds() const
{
    std::ostringstream oss;
    oss.precision(3);
    oss << std::fixed << (duration_ms() / 1000.0) << " s";
    return oss.str();
}

void TimeMeasurement::reset()
{
    start_ = clock::now();
    end_ = start_;
}

} // namespace Utils
} // namespace Antares
