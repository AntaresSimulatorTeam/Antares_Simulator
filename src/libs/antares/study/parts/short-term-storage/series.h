//TODO NOTICE

#pragma once
#include <vector>
#include <string>

namespace Antares::Data::ShortTermStorage {
  class Series {
    bool validate() const;
    bool loadFromFolder(const std::string& folder);
    std::vector<double> maxInjection;
    std::vector<double> maxWithdrawal;
    std::vector<double> inflows;
    std::vector<double> lowerRuleCurve;
    std::vector<double> upperRuleCurve;
  };
}
