//
// Created by marechaljas on 06/11/23.
//

#pragma once

#include "antares/solver/simulation/LpsFromAntares.h"
namespace Antares
{
namespace Solver
{

class OutputWriter
{
public:
    void PrintMe();
    LpsFromAntares lps;
};

} // namespace Solver
} // namespace Antares
