#pragma once

#include <string>

namespace Antares::Data
{
class LTStorageClusterReserveParticipation
{
public:
    LTStorageClusterReserveParticipation(const std::string& reserveName, 
                                         float maxTurbining, 
                                         float maxPumping, 
                                         float participationCost)
        : reserveName(reserveName), 
          maxTurbining(maxTurbining), 
          maxPumping(maxPumping), 
          participationCost(participationCost) {}

    std::string reserveName;
    float maxTurbining;
    float maxPumping;
    float participationCost;
};
} // namespace Antares::Data